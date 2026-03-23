#!/usr/bin/env python3
"""
Render a dataset sequence to a video file.

Each frame shows:
  • LiDAR scan points        — grey dots
  • GT wheelchair (wc)       — orange circles
  • GT walker (wa)           — purple circles
  • GT person (wp)           — green circles
  • AlgorithmicDetector      — red triangles       (on by default)
  • Any trained NN detector  — coloured circles,
                               radius ∝ agnostic confidence
                               (enabled per detector via CLI flags)

Annotations outside the laser FoV are drawn faded (alpha=0.25).

Output formats
--------------
  mp4   — H.264 via cv2.VideoWriter  (default, requires OpenCV)
  gif   — animated GIF via Pillow

Usage
-----
  # FROG test set, algorithmic only, MP4
  python render_video.py --dataset frog

  # Add a trained DR-SPAAM detector
  python render_video.py --dataset frog --drspaam weights_drspaam_frog.pth

  # Multiple detectors at once
  python render_video.py --dataset frog \\
      --drspaam  weights_drspaam_frog.pth \\
      --spacetime-cnn weights_spacetime_cnn_frog.pth

  # Disable the algorithmic detector, use only NN
  python render_video.py --no-algo --drow weights_drow.pth

  # DROW dataset, single sequence, GIF output
  python render_video.py --dataset drow --seq 0 --format gif

  # Default: native dataset annotation rate (real-time speed)
  python render_video.py --dataset frog

  # Custom FPS, limit to first 200 frames
  python render_video.py --dataset frog --fps 10 --max-frames 200

  # Output to a specific path
  python render_video.py --dataset frog --out my_video.mp4
"""

import argparse
import sys
import tempfile
from pathlib import Path
from types import SimpleNamespace

import numpy as np
import matplotlib
matplotlib.use("Agg")          # non-interactive backend for off-screen rendering
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
plt.style.use("ggplot")

sys.path.insert(0, str(Path(__file__).parent))
from follow_the_drow.detectors import AlgorithmicDetector
from follow_the_drow.utils.drow_utils import (
    laser_angles, laser_minimum, laser_maximum, laser_increment,
    rphi_to_xy, cutout, aligned_scan_xyz, _win2global, votes_to_detections,
)
import torch
import torch.nn.functional as F
from tqdm import tqdm


# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

_RESULT_CONF = {
    "blur_sigma":          2.23409276092903,
    "blur_win":            11,
    "bin_size":            0.04566379529562327,
    "vote_collect_radius": 0.6351825665330302,
    "min_thresh":          0.0027015322261551397,
    "class_weights":       [0.89740097838073, 0.3280190481521334, 0.4575675717820713],
}

# Colors for successive NN detectors (cycled if more than 6 are enabled)
_NN_COLORS = ["#4488FF", "#FF44AA", "#44FFCC", "#FFEE44", "#FF7744", "#AA44FF"]

# Marker size for scan points (pixels²)
_SCAN_S = 2


# ---------------------------------------------------------------------------
# Dataset / config
# ---------------------------------------------------------------------------

def _setup(args):
    if args.dataset == "frog":
        from follow_the_drow.datasets import FROG_Dataset, frog_laser_angles
        print(f"Loading FROG dataset (split='{args.split}') …")
        dataset = FROG_Dataset(split=args.split)
        cfg = SimpleNamespace(
            name="frog",
            angles_fn=frog_laser_angles,
            fov_min=FROG_Dataset.LASER_MIN_ANGLE,
            fov_max=FROG_Dataset.LASER_MAX_ANGLE,
            laser_inc=FROG_Dataset.LASER_INCREMENT,
            scan_rate=40,
        )
    else:
        from follow_the_drow.datasets import DROW_Dataset
        _drow_splits = {"train", "val", "test"}
        if args.split not in _drow_splits:
            raise ValueError(f"DROW split must be train/val/test, got '{args.split}'")
        print(f"Loading DROW dataset (split='{args.split}') …")
        dataset = DROW_Dataset(dataset=args.split)
        cfg = SimpleNamespace(
            name="drow",
            angles_fn=laser_angles,
            fov_min=laser_minimum,
            fov_max=laser_maximum,
            laser_inc=laser_increment,
            scan_rate=25,
        )

    # Derive native annotation FPS from actual annotation density in the data
    n_scans = sum(len(s) for s in dataset.scan_id)
    n_anns  = sum(len(d) for d in dataset.det_id)
    ann_density = n_anns / max(1, n_scans)
    cfg.native_fps = max(1, round(cfg.scan_rate * ann_density))
    print(f"  {len(dataset.scan_id)} sequence(s), {n_anns} annotated frames")
    print(f"  Scan rate: {cfg.scan_rate} Hz  |  annotation density: "
          f"{ann_density:.1%}  ->  native ~{cfg.native_fps} fps")
    return dataset, cfg


# ---------------------------------------------------------------------------
# AlgorithmicDetector runner (stateful: one instance per sequence)
# ---------------------------------------------------------------------------

class _AlgoRunner:
    """Wraps AlgorithmicDetector with per-sequence state reset."""
    def __init__(self):
        self._instances = {}    # seq -> AlgorithmicDetector

    def __call__(self, dataset, seq, det):
        if seq not in self._instances:
            self._instances[seq] = AlgorithmicDetector(verbose=False)
        algo = self._instances[seq]
        iscan = dataset.idet2iscan[seq][det]
        scans_hist, odoms_hist = dataset.get_scan(seq, iscan, algo.time_frame)
        result = np.array(algo.forward_one(scans_hist[-1], odoms_hist[-1]["xya"]))
        return result.reshape(-1, 2) if result.ndim == 1 and result.size > 0 else result


# ---------------------------------------------------------------------------
# NN detector runner
# ---------------------------------------------------------------------------

def _load_nn_runner(det_name: str, weights_path: Path, cfg, device: str):
    """
    Load a trained NN detector checkpoint and return a runner callable
    (dataset, seq, det) -> list of (x, y, probs) tuples.
    """
    from train import _default_args, _build_model, load_checkpoint

    args = _default_args(detector=det_name)
    net  = _build_model(args).to(device)
    load_checkpoint(weights_path, net)
    net.eval()

    input_mode = getattr(net, "INPUT_MODE", "cutout")

    def _runner(dataset, seq, det):
        iscan = dataset.idet2iscan[seq][det]
        scan  = dataset.scans[seq][iscan]
        scans_hist, odoms_hist = dataset.get_scan(seq, iscan, 5)
        angles = cfg.angles_fn(len(scan))

        if input_mode == "full_scan":
            xyz = aligned_scan_xyz(scans_hist, odoms_hist, angles,
                                   laser_inc=cfg.laser_inc)
            x_in = torch.from_numpy(xyz.transpose(1, 0, 2)).to(device)
        else:
            from follow_the_drow.detectors import DrowDetector
            nsamp = getattr(net, "N_SAMP", DrowDetector.N_SAMP)
            cut   = cutout(scans_hist, odoms_hist, len(scan),
                           nsamp=nsamp, laserIncrement=cfg.laser_inc)
            x_in  = torch.from_numpy(cut).to(device)

        with torch.no_grad():
            logits, vpred = net(x_in)

        confs = F.softmax(logits, dim=-1).cpu().numpy()
        votes = vpred.cpu().numpy()

        r_new, phi_new = _win2global(
            scan[None], angles[None],
            votes[None, :, 0], votes[None, :, 1],
        )
        x_v = r_new * -np.sin(phi_new)
        y_v = r_new *  np.cos(phi_new)
        return votes_to_detections(x_v, y_v, confs[None], **_RESULT_CONF)[0]

    return _runner


# ---------------------------------------------------------------------------
# Frame rendering
# ---------------------------------------------------------------------------

def _split_fov(anns, fov_min, fov_max):
    in_, out_ = [], []
    for r, phi in anns:
        (in_ if fov_min <= phi <= fov_max else out_).append((r, phi))
    return in_, out_


def _draw_fov_blind_spot(ax, fov_min, fov_max, r_max=12):
    theta = np.linspace(fov_max, 2 * np.pi + fov_min, 120)
    xs = np.concatenate([[0], r_max * -np.sin(theta), [0]])
    ys = np.concatenate([[0], r_max *  np.cos(theta), [0]])
    ax.fill(xs, ys, color="gray", alpha=0.10, zorder=0)


def _render_frame(fig, ax, dataset, cfg, seq, det,
                  detectors, frame_idx, total_frames):
    """
    Draw one annotated frame onto *ax* and return the figure as an RGB array.

    detectors : list of (label, color, runner_fn, is_algo)
    """
    ax.cla()
    ax.set_aspect("equal")
    ax.set_xlim(-7, 7)
    ax.set_ylim(-2, 10)
    ax.set_xlabel("x (m)", fontsize=8)
    ax.set_ylabel("y (m)", fontsize=8)

    iscan  = dataset.idet2iscan[seq][det]
    scan   = np.asarray(dataset.scans[seq][iscan], dtype=np.float64)
    angles = cfg.angles_fn(len(scan))

    # --- scan points ---
    xs = scan * -np.sin(angles)
    ys = scan *  np.cos(angles)
    _draw_fov_blind_spot(ax, cfg.fov_min, cfg.fov_max)
    ax.scatter(xs, ys, s=_SCAN_S, c="#909090", zorder=1)

    # --- GT annotations ---
    gt_spec = [
        ("wc", "#FF8C00", dataset.det_wc[seq][det]),
        ("wa", "#9B30FF", dataset.det_wa[seq][det]),
        ("wp", "#00CC44", dataset.det_wp[seq][det]),
    ]
    n_gt = 0
    for label, color, anns in gt_spec:
        in_fov, out_fov = _split_fov(anns, cfg.fov_min, cfg.fov_max)
        n_gt += len(in_fov) + len(out_fov)
        for r, phi in in_fov:
            x, y = rphi_to_xy(r, phi)
            ax.add_patch(plt.Circle((x, y), 0.30, fill=False,
                                    edgecolor=color, lw=1.8, zorder=3))
            ax.plot(x, y, "x", color=color, ms=5, mew=1.5, zorder=3)
        for r, phi in out_fov:
            x, y = rphi_to_xy(r, phi)
            ax.add_patch(plt.Circle((x, y), 0.30, fill=False,
                                    edgecolor=color, lw=1.0,
                                    alpha=0.25, zorder=2))

    # --- detections ---
    det_counts = {}
    for label, color, runner, is_algo in detectors:
        result = runner(dataset, seq, det)
        if is_algo:
            xy = result
            n  = len(xy)
            if n > 0:
                ax.scatter(xy[:, 0], xy[:, 1], s=100, marker="^",
                           c=color, zorder=5)
        else:
            n = len(result)
            for x_d, y_d, probs in result:
                agnostic = float(np.sum(probs[1:]))
                radius   = max(0.12, agnostic * 0.55)
                ax.add_patch(plt.Circle((x_d, y_d), radius, fill=False,
                                        edgecolor=color, lw=2.0, zorder=5))
            if n:
                xs_d = [d[0] for d in result]
                ys_d = [d[1] for d in result]
                ax.scatter(xs_d, ys_d, s=30, c=color, zorder=6)
        det_counts[label] = n

    # Robot position
    ax.plot(0, 0, "k+", ms=12, mew=2, zorder=7)

    # --- legend — markers mirror exactly what is drawn on the plot ---
    handles = [
        # scan: small filled grey dot
        Line2D([0], [0], linestyle="none", marker="o",
               markerfacecolor="#909090", color="w", markersize=5, label="scan"),
        # GT classes: X mark (centre of the drawn circle)
        Line2D([0], [0], linestyle="none", marker="x", color="#00CC44",
               markersize=8, markeredgewidth=2, label="GT person (wp)"),
        Line2D([0], [0], linestyle="none", marker="x", color="#FF8C00",
               markersize=8, markeredgewidth=2, label="GT wheelchair (wc)"),
        Line2D([0], [0], linestyle="none", marker="x", color="#9B30FF",
               markersize=8, markeredgewidth=2, label="GT walker (wa)"),
        # faded entry for out-of-FoV annotations
        Line2D([0], [0], linestyle="none", marker="x", color="#00CC44",
               alpha=0.30, markersize=7, markeredgewidth=1.2,
               label="  outside FoV (faded)"),
    ]
    for label, color, _, is_algo in detectors:
        if is_algo:
            # algorithmic detector: filled triangle
            h = Line2D([0], [0], linestyle="none", marker="^",
                       markerfacecolor=color, color="w",
                       markersize=10, label=label)
        else:
            # NN detector: open circle (matches the drawn confidence circle)
            h = Line2D([0], [0], linestyle="none", marker="o",
                       markerfacecolor="none", markeredgecolor=color,
                       markeredgewidth=2.0, color="w",
                       markersize=10, label=label)
        handles.append(h)
    ax.legend(handles=handles, fontsize=7, loc="upper right",
              framealpha=0.80, edgecolor="#cccccc")

    # --- title ---
    count_str = "  ".join(f"{lbl}={n}" for lbl, n in det_counts.items())
    ax.set_title(
        f"[{frame_idx + 1}/{total_frames}]  seq={seq}  det={det}  "
        f"GT={n_gt}  {count_str}",
        fontsize=9,
    )

    fig.canvas.draw()
    buf = np.frombuffer(fig.canvas.buffer_rgba(), dtype=np.uint8)
    w, h = fig.canvas.get_width_height()
    return buf.reshape(h, w, 4)[..., :3]


# ---------------------------------------------------------------------------
# Video writer helpers
# ---------------------------------------------------------------------------

def _open_mp4_writer(path: Path, width: int, height: int, fps: int):
    import cv2
    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    writer = cv2.VideoWriter(str(path), fourcc, fps, (width, height))
    if not writer.isOpened():
        raise RuntimeError(f"cv2.VideoWriter could not open '{path}'. "
                           "Try --format gif.")
    return writer


def _write_mp4_frame(writer, rgb_frame):
    import cv2
    writer.write(cv2.cvtColor(rgb_frame, cv2.COLOR_RGB2BGR))


def _open_gif_writer(path: Path, fps: int):
    try:
        import imageio
    except ImportError:
        raise ImportError("GIF output requires imageio: pip install imageio")
    return imageio.get_writer(str(path), mode="I",
                              duration=1.0 / fps, loop=0)


def _write_gif_frame(writer, rgb_frame):
    writer.append_data(rgb_frame)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _parse():
    p = argparse.ArgumentParser(
        description="Render dataset frames to a video.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    # Dataset
    p.add_argument("--dataset",      default="frog", choices=["drow", "frog"])
    p.add_argument("--split",        default="test",
                   help="Dataset split: train / val / test (default: test)")
    p.add_argument("--seq",          type=int, default=None,
                   help="Render only this sequence (default: all sequences)")
    p.add_argument("--max-frames",   type=int, default=0,
                   help="Stop after this many frames (0 = no limit)")

    # Detectors
    p.add_argument("--no-algo",      action="store_true",
                   help="Disable the AlgorithmicDetector")
    p.add_argument("--drow",               metavar="WEIGHTS", default=None)
    p.add_argument("--drspaam",            metavar="WEIGHTS", default=None)
    p.add_argument("--fullscan-cnn",       metavar="WEIGHTS", default=None)
    p.add_argument("--spacetime-cnn",      metavar="WEIGHTS", default=None)
    p.add_argument("--fullscan-transformer", metavar="WEIGHTS", default=None)

    # Output
    p.add_argument("--out",    type=Path, default=None,
                   help="Output path (default: render_<dataset>_<split>.mp4/gif)")
    p.add_argument("--format", choices=["mp4", "gif"], default="mp4")
    p.add_argument("--fps",    type=int, default=None,
                   help="Output FPS (default: native dataset annotation rate)")
    p.add_argument("--dpi",    type=int, default=100,
                   help="Figure DPI — controls resolution (default: 100)")
    return p.parse_args()


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------

def main():
    args = _parse()

    dataset, cfg = _setup(args)

    # Build ordered frame list
    seqs = ([args.seq] if args.seq is not None
            else range(len(dataset.det_id)))
    frames = [(s, d)
              for s in seqs
              for d in range(len(dataset.det_id[s]))]
    if not frames:
        print("No annotated frames found.")
        sys.exit(1)
    if args.max_frames > 0:
        frames = frames[:args.max_frames]
    print(f"  {len(frames)} frames to render")

    # Device (CPU is fine for inference; GRU models need CPU on DirectML)
    try:
        import torch_directml
        _dml = True
    except ImportError:
        _dml = False
    device = "cpu"   # rendering is bottlenecked by matplotlib, not inference
    print(f"  Inference device: {device}\n")

    # Build detector list: (label, color, runner, is_algo)
    detectors = []
    color_iter = iter(_NN_COLORS)

    if not args.no_algo:
        detectors.append(("algorithmic", "#EE3333", _AlgoRunner(), True))

    nn_map = [
        ("drow",                 args.drow),
        ("drspaam",              args.drspaam),
        ("fullscan_cnn",         args.fullscan_cnn),
        ("spacetime_cnn",        args.spacetime_cnn),
        ("fullscan_transformer", args.fullscan_transformer),
    ]
    for det_name, weights in nn_map:
        if weights is None:
            continue
        w = Path(weights)
        if not w.exists():
            print(f"  [WARN] weights not found: {w} — skipping {det_name}")
            continue
        print(f"  Loading {det_name} from {w} …")
        color   = next(color_iter, "#AAAAAA")
        runner  = _load_nn_runner(det_name, w, cfg, device)
        label   = det_name.replace("_", " ")
        detectors.append((label, color, runner, False))

    if not detectors:
        print("  [WARN] No detectors enabled. Use --no-algo only with a --<model> flag.")

    # Resolve FPS
    if args.fps is not None:
        fps = args.fps
    else:
        fps = cfg.native_fps

    # Output path
    suffix = f".{args.format}"
    out_path = args.out or Path(f"render_{cfg.name}_{args.split}{suffix}")
    print(f"\n  Output : {out_path}  ({fps} fps, {args.dpi} dpi)")
    print(f"  Format : {args.format}\n")

    # Figure setup (Agg backend — no display needed)
    fig_size = (9, 9)
    fig, ax = plt.subplots(figsize=fig_size, dpi=args.dpi)
    plt.tight_layout(pad=1.5)

    # Determine pixel dimensions from the first rendered frame
    probe = _render_frame(fig, ax, dataset, cfg, *frames[0], detectors, 0, len(frames))
    h, w = probe.shape[:2]
    print(f"  Frame size: {w}×{h} px\n")

    # Open video writer
    if args.format == "mp4":
        writer   = _open_mp4_writer(out_path, w, h, fps)
        write_fn = _write_mp4_frame
    else:
        writer   = _open_gif_writer(out_path, fps)
        write_fn = _write_gif_frame

    # Write frames
    write_fn(writer, probe)   # first frame already rendered above
    try:
        for i, (seq, det) in enumerate(
                tqdm(frames[1:], initial=1, total=len(frames),
                     unit="fr", desc="Rendering"),
                start=1):
            frame = _render_frame(fig, ax, dataset, cfg, seq, det,
                                  detectors, i, len(frames))
            write_fn(writer, frame)
    finally:
        if args.format == "mp4":
            writer.release()
        else:
            writer.close()
        plt.close(fig)

    print(f"\nDone -> {out_path}")


if __name__ == "__main__":
    main()
