#!/usr/bin/env python3
"""
Interactive animated visualization of DROW or FROG dataset.

Shows every annotated frame as a live animation with:
  • Raw LiDAR scan           (grey dots)
  • Ground-truth annotations (green circles — solid=in FoV, faded=behind robot)
  • Algorithmic detections   (red triangles)
  • NN/DROW detections       (blue circles, radius ∝ agnostic confidence)
    — only when a weights file is provided (DROW trained on 450-beam scans
      is incompatible with FROG's 720 beams unless re-trained)

Keyboard controls
-----------------
  Space / p   toggle play / pause
  → / .       step forward one frame
  ← / ,       step backward one frame
  + / =       speed up
  - / _       slow down
  r           restart from frame 0
  s           save current frame as PNG
  q / Escape  quit

Usage
-----
  # DROW test set, no NN (algorithmic only)
  python visualize.py

  # DROW with NN detections
  python visualize.py --weights weights_drow.pth

  # FROG test set (algorithmic only — NN needs FROG-trained weights)
  python visualize.py --dataset frog

  # FROG with NN detections from a FROG-trained checkpoint
  python visualize.py --dataset frog --weights weights_frog.pth

  # Specific sequence, start at frame 50
  python visualize.py --seq 0 --start 50

  # Save every frame to disk (non-interactive)
  python visualize.py --save-all --out-dir plots/vis
"""

import argparse
import sys
import time
from pathlib import Path
from types import SimpleNamespace

import numpy as np
import matplotlib
matplotlib.rcParams["font.family"] = "DejaVu Sans"
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
plt.style.use("ggplot")

from follow_the_drow.detectors import AlgorithmicDetector, DrowDetector
from follow_the_drow.utils.drow_utils import (
    laser_angles, laser_minimum, laser_maximum, laser_increment,
    rphi_to_xy, cutout, _win2global, votes_to_detections,
)


# ---------------------------------------------------------------------------
# Detector post-processing (same hyperparams as verify_dataset / compare)
# ---------------------------------------------------------------------------

_RESULT_CONF = {
    "blur_sigma":          2.23409276092903,
    "blur_win":            11,
    "bin_size":            0.04566379529562327,
    "vote_collect_radius": 0.6351825665330302,
    "min_thresh":          0.0027015322261551397,
    "class_weights":       [0.89740097838073, 0.3280190481521334, 0.4575675717820713],
}


# ---------------------------------------------------------------------------
# Dataset + config setup
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
            n_beams=720,
        )
    else:
        from follow_the_drow.datasets import DROW_Dataset
        print("Loading DROW test set …")
        dataset = DROW_Dataset()
        cfg = SimpleNamespace(
            name="drow",
            angles_fn=laser_angles,
            fov_min=laser_minimum,
            fov_max=laser_maximum,
            laser_inc=laser_increment,
            n_beams=450,
        )
    print(f"  {len(dataset.scan_id)} sequence(s) loaded")
    return dataset, cfg


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _ann_to_xy(anns):
    if len(anns) == 0:
        return np.array([]), np.array([])
    xy = np.array([rphi_to_xy(r, phi) for r, phi in anns])
    return xy[:, 0], xy[:, 1]


def _split_fov(anns, fov_min, fov_max):
    in_, out_ = [], []
    for r, phi in anns:
        (in_ if fov_min <= phi <= fov_max else out_).append((r, phi))
    return in_, out_


def _draw_fov_blind_spot(ax, fov_min, fov_max, r_max=12):
    theta = np.linspace(fov_max, 2 * np.pi + fov_min, 120)
    xs = np.concatenate([[0], r_max * -np.sin(theta), [0]])
    ys = np.concatenate([[0], r_max *  np.cos(theta), [0]])
    ax.fill(xs, ys, color="gray", alpha=0.13, zorder=0)


# ---------------------------------------------------------------------------
# Per-frame detector runners
# ---------------------------------------------------------------------------

def _run_algo(algo, dataset, seq, det):
    iscan = dataset.idet2iscan[seq][det]
    scans_hist, odoms_hist = dataset.get_scan(seq, iscan, algo.time_frame)
    result = np.array(algo.forward_one(scans_hist[-1], odoms_hist[-1]["xya"]))
    return result.reshape(-1, 2) if result.ndim == 1 else result


def _run_drow(drow, dataset, seq, det, cfg):
    iscan = dataset.idet2iscan[seq][det]
    scan  = dataset.scans[seq][iscan]
    scans_hist, odoms_hist = dataset.get_scan(seq, iscan, drow.time_frame)

    cut = cutout(scans_hist, odoms_hist, len(scan),
                 nsamp=drow.N_SAMP, laserIncrement=cfg.laser_inc)
    confs, votes = drow.forward_one(cut)

    angles = cfg.angles_fn(len(scan))
    r_new, phi_new = _win2global(
        scan[None], angles[None],
        votes[None, :, 0], votes[None, :, 1],
    )
    x_v = r_new * -np.sin(phi_new)
    y_v = r_new *  np.cos(phi_new)
    return votes_to_detections(x_v, y_v, confs[None], **_RESULT_CONF)[0]


# ---------------------------------------------------------------------------
# Frame index helpers
# ---------------------------------------------------------------------------

def _build_frame_list(dataset, seq=None):
    """Return list of (seq, det_idx) pairs."""
    frames = []
    seqs = [seq] if seq is not None else range(len(dataset.det_id))
    for s in seqs:
        for d in range(len(dataset.det_id[s])):
            frames.append((s, d))
    return frames


# ---------------------------------------------------------------------------
# Main visualizer class
# ---------------------------------------------------------------------------

class Visualizer:
    _SPEEDS = [0.05, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0]   # seconds per frame

    def __init__(self, dataset, cfg, drow, frames, args):
        self.dataset = dataset
        self.cfg     = cfg
        self.drow    = drow
        self.frames  = frames
        self.args    = args
        self.idx     = max(0, min(args.start, len(frames) - 1))

        # Detector cache  {(seq, det): result}
        self._algo_cache = {}
        self._drow_cache = {}

        # Per-sequence algorithmic detector instances
        self._algos = {}

        # Playback state
        self._playing   = not args.save_all
        self._speed_idx = 4   # 1.0 s/frame default
        self._last_t    = 0.0

        self._setup_figure()

    # ------------------------------------------------------------------

    def _algo_for_seq(self, seq):
        """Return a stateful AlgorithmicDetector for this sequence."""
        if seq not in self._algos:
            self._algos[seq] = AlgorithmicDetector(verbose=False)
        return self._algos[seq]

    def _get_algo(self, seq, det):
        key = (seq, det)
        if key not in self._algo_cache:
            self._algo_cache[key] = _run_algo(
                self._algo_for_seq(seq), self.dataset, seq, det)
        return self._algo_cache[key]

    def _get_drow(self, seq, det):
        if self.drow is None:
            return []
        key = (seq, det)
        if key not in self._drow_cache:
            self._drow_cache[key] = _run_drow(
                self.drow, self.dataset, seq, det, self.cfg)
        return self._drow_cache[key]

    # ------------------------------------------------------------------

    def _setup_figure(self):
        self.fig, self.ax = plt.subplots(figsize=(9, 9))
        self.fig.canvas.mpl_connect("key_press_event", self._on_key)
        self.fig.canvas.mpl_connect("close_event",     self._on_close)
        self._running = True
        plt.tight_layout()

    def _on_close(self, _):
        self._running = False

    def _on_key(self, event):
        k = event.key
        if k in ("q", "escape"):
            self._running = False
            plt.close(self.fig)
        elif k in (" ", "p"):
            self._playing = not self._playing
        elif k in ("right", "."):
            self._step(+1)
        elif k in ("left", ","):
            self._step(-1)
        elif k in ("+", "="):
            self._speed_idx = max(0, self._speed_idx - 1)
        elif k in ("-", "_"):
            self._speed_idx = min(len(self._SPEEDS) - 1, self._speed_idx + 1)
        elif k == "r":
            self.idx = 0
            self._draw()
        elif k == "s":
            self._save_frame()

    def _step(self, delta):
        self.idx = (self.idx + delta) % len(self.frames)
        self._draw()

    def _save_frame(self):
        out = Path(self.args.out_dir)
        out.mkdir(parents=True, exist_ok=True)
        seq, det = self.frames[self.idx]
        fname = out / f"vis_{self.cfg.name}_seq{seq}_det{det:05d}.png"
        self.fig.savefig(fname, dpi=120, bbox_inches="tight")
        print(f"  saved {fname}")

    # ------------------------------------------------------------------

    def _draw(self):
        ax  = self.ax
        cfg = self.cfg
        seq, det = self.frames[self.idx]

        iscan = self.dataset.idet2iscan[seq][det]
        scan  = np.asarray(self.dataset.scans[seq][iscan], dtype=np.float64)
        angles = cfg.angles_fn(len(scan))

        # Cartesian scan points
        xs = scan * -np.sin(angles)
        ys = scan *  np.cos(angles)

        # Ground truth
        wc_in, wc_out = _split_fov(self.dataset.det_wc[seq][det], cfg.fov_min, cfg.fov_max)
        wa_in, wa_out = _split_fov(self.dataset.det_wa[seq][det], cfg.fov_min, cfg.fov_max)
        wp_in, wp_out = _split_fov(self.dataset.det_wp[seq][det], cfg.fov_min, cfg.fov_max)

        # Detections
        algo_xy = self._get_algo(seq, det)   # (N, 2)
        drow_dets = self._get_drow(seq, det) # list of (x, y, probs)

        # --- Draw ---
        ax.cla()
        ax.set_aspect("equal")
        ax.set_xlim(-7, 7)
        ax.set_ylim(-2, 10)
        ax.set_xlabel("x (m)")
        ax.set_ylabel("y (m)")

        # FoV blind spot
        _draw_fov_blind_spot(ax, cfg.fov_min, cfg.fov_max)

        # Scan
        ax.scatter(xs, ys, s=2, c="silver", zorder=1, label="scan")

        # GT annotations per class
        _cls_kwargs = [
            ("wc", "tab:orange", wc_in, wc_out),
            ("wa", "tab:purple", wa_in, wa_out),
            ("wp", "tab:green",  wp_in, wp_out),
        ]
        for label, color, in_fov, out_fov in _cls_kwargs:
            for r, phi in in_fov:
                x, y = rphi_to_xy(r, phi)
                ax.add_patch(plt.Circle((x, y), 0.3, fill=False,
                                        edgecolor=color, lw=1.8, zorder=3))
                ax.plot(x, y, "x", color=color, ms=5, zorder=3)
            for r, phi in out_fov:
                x, y = rphi_to_xy(r, phi)
                ax.add_patch(plt.Circle((x, y), 0.3, fill=False,
                                        edgecolor=color, lw=1.0,
                                        alpha=0.3, zorder=2))

        # Algorithmic detections
        if len(algo_xy) > 0:
            ax.scatter(algo_xy[:, 0], algo_xy[:, 1],
                       s=120, marker="^", c="tab:red", zorder=4,
                       label="algorithmic")

        # DROW/NN detections
        for x_d, y_d, probs in drow_dets:
            agnostic = float(1.0 - probs[0])   # 1 - p(background)
            r_circ   = max(0.15, agnostic * 0.6)
            ax.add_patch(plt.Circle((x_d, y_d), r_circ, fill=False,
                                     edgecolor="tab:blue", lw=2.0, zorder=4))
        if drow_dets:
            ax.scatter([d[0] for d in drow_dets],
                       [d[1] for d in drow_dets],
                       s=40, c="tab:blue", zorder=5, label="NN detector")

        # Robot at origin
        ax.plot(0, 0, "k+", ms=14, zorder=6)

        # Legend
        legend_handles = [
            mpatches.Patch(color="silver",      label="scan"),
            mpatches.Patch(color="tab:green",   label="GT person (wp)"),
            mpatches.Patch(color="tab:orange",  label="GT wheelchair (wc)"),
            mpatches.Patch(color="tab:purple",  label="GT walker (wa)"),
            mpatches.Patch(color="tab:red",     label="algorithmic"),
        ]
        if self.drow is not None:
            legend_handles.append(
                mpatches.Patch(color="tab:blue", label="NN detector"))
        ax.legend(handles=legend_handles, fontsize=8,
                  loc="upper right", framealpha=0.7)

        speed = self._SPEEDS[self._speed_idx]
        status = "▶" if self._playing else "⏸"
        n_ann = (len(wc_in) + len(wc_out) +
                 len(wa_in) + len(wa_out) +
                 len(wp_in) + len(wp_out))
        ax.set_title(
            f"{status}  [{self.idx + 1}/{len(self.frames)}]  "
            f"seq={seq}  det={det}  GT={n_ann}  "
            f"algo={len(algo_xy)}  NN={len(drow_dets)}  "
            f"speed={speed:.2f}s/fr  [space=play/pause  ←→=step  ±=speed  s=save  q=quit]",
            fontsize=9
        )

        self.fig.canvas.draw_idle()

    # ------------------------------------------------------------------

    def run(self):
        if self.args.save_all:
            self._run_save_all()
        else:
            self._run_interactive()

    def _run_interactive(self):
        self._draw()
        plt.pause(0.05)

        while self._running:
            now = time.time()
            speed = self._SPEEDS[self._speed_idx]

            if self._playing and (now - self._last_t) >= speed:
                self._step(+1)
                self._last_t = now

            plt.pause(0.05)

        plt.close("all")

    def _run_save_all(self):
        from tqdm import tqdm
        print(f"Saving {len(self.frames)} frames to '{self.args.out_dir}' …")
        for i in tqdm(range(len(self.frames)), unit="fr"):
            self.idx = i
            self._draw()
            self._save_frame()
        print("Done.")
        plt.close("all")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _parse():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--dataset",   default="drow", choices=["drow", "frog"])
    p.add_argument("--split",     default="test",
                   help="Dataset split: train / val / test (FROG only)")
    p.add_argument("--seq",       type=int, default=None,
                   help="Show only this sequence index (default: all)")
    p.add_argument("--start",     type=int, default=0,
                   help="Start at this frame index (default: 0)")
    p.add_argument("--weights",   type=str, default=None,
                   help="Path to DrowDetector checkpoint (.pth). "
                        "If omitted, NN detections are skipped.")
    p.add_argument("--save-all",  action="store_true",
                   help="Non-interactive: save every frame to disk")
    p.add_argument("--out-dir",   default="plots/vis",
                   help="Output directory for saved frames (default: plots/vis)")
    return p.parse_args()


def main():
    args = _parse()

    dataset, cfg = _setup(args)

    # Build the ordered frame list
    frames = _build_frame_list(dataset, seq=args.seq)
    if not frames:
        print("No annotated frames found.")
        sys.exit(1)
    print(f"  {len(frames)} annotated frames to visualize\n")

    # Optional NN detector
    drow = None
    if args.weights:
        w = Path(args.weights)
        if not w.exists():
            print(f"WARNING: weights file '{w}' not found — skipping NN detector.")
        else:
            print(f"Loading DrowDetector weights from '{w}' …")
            drow = DrowDetector.init(verbose=False)
            import torch
            ckpt = torch.load(str(w), map_location="cpu")
            state = ckpt.get("model_state_dict", ckpt)
            drow.load_state_dict(state)
            drow.eval()
            print("  NN detector ready.\n")
    else:
        print("  (no --weights given — NN detections disabled)\n")

    vis = Visualizer(dataset, cfg, drow, frames, args)
    vis.run()


if __name__ == "__main__":
    main()
