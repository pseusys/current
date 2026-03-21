#!/usr/bin/env python3
"""
Side-by-side comparison of ML (DROW) and algorithmic detections.

For every annotated frame this script runs *both* detectors and saves a plot
showing:
  - The raw LiDAR scan
  - Ground-truth annotations  (X marks — possibly mis-placed)
  - DROW detections           (circles, radius = detection probability)
  - Algorithmic detections    (triangles)

This makes it easy to see:
  1. Which detector finds the *visually obvious* human clusters.
  2. Whether the two detectors agree (if yes, they are both likely right).
  3. Whether GT annotations agree with either detector (if no, annotations are wrong).

Usage
-----
  # DROW — one frame (seq 1, det 3 — the mis-annotated frame from the paper)
  python compare_detectors.py --seq 1 --det 3

  # FROG — test set
  python compare_detectors.py --dataset frog

  # All frames for all sequences
  python compare_detectors.py --all-seqs --all-frames

  # Print agreement statistics
  python compare_detectors.py --stats

  # Skip slow DROW forward pass (algorithmic only)
  python compare_detectors.py --no-drow
"""

import argparse
from pathlib import Path
from types import SimpleNamespace

import numpy as np
import matplotlib
matplotlib.rcParams["font.family"] = "DejaVu Sans"
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
plt.style.use("ggplot")

from follow_the_drow.datasets import DROW_Dataset
from follow_the_drow.detectors import AlgorithmicDetector, DrowDetector
from follow_the_drow.utils.drow_utils import (
    laser_angles, laser_minimum, laser_maximum, laser_increment,
    rphi_to_xy, cutout, _win2global, votes_to_detections,
)


# Postprocessing hyperparameters (identical to verify_dataset.py)
_RESULT_CONF = {
    "blur_sigma":          2.23409276092903,
    "blur_win":            11,
    "bin_size":            0.04566379529562327,
    "vote_collect_radius": 0.6351825665330302,
    "min_thresh":          0.0027015322261551397,
    "class_weights":       [0.89740097838073, 0.3280190481521334, 0.4575675717820713],
}


# ---------------------------------------------------------------------------
# Dataset setup
# ---------------------------------------------------------------------------

def _setup(args) -> tuple:
    """Load the requested dataset and return (dataset, cfg)."""
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
        )
    else:
        print("Loading DROW test set …")
        dataset = DROW_Dataset()
        cfg = SimpleNamespace(
            name="drow",
            angles_fn=laser_angles,
            fov_min=laser_minimum,
            fov_max=laser_maximum,
            laser_inc=laser_increment,
        )
    print(f"  {len(dataset.scan_id)} sequence(s) loaded\n")
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
    ax.fill(xs, ys, color="gray", alpha=0.13, zorder=0, label="laser blind spot")


def _first_annotated(dataset, seq):
    for i in range(len(dataset.det_id[seq])):
        if (len(dataset.det_wc[seq][i])
                + len(dataset.det_wa[seq][i])
                + len(dataset.det_wp[seq][i])) > 0:
            return i
    return None


# ---------------------------------------------------------------------------
# Per-frame runner functions
# ---------------------------------------------------------------------------

def run_drow_frame(drow: DrowDetector, dataset, seq: int, det: int, cfg=None):
    """
    Run DROW on a single annotated frame.

    Returns a list of (x, y, probs) tuples where probs is a 4-element array
    [agnostic, wc, wa, wp].  Returns an empty list if no detection passes the
    threshold.
    """
    angles_fn  = cfg.angles_fn  if cfg else laser_angles
    laser_inc  = cfg.laser_inc  if cfg else laser_increment

    iscan  = dataset.idet2iscan[seq][det]
    scan   = dataset.scans[seq][iscan]
    scans_hist, odoms_hist = dataset.get_scan(seq, iscan, drow.time_frame)

    cut = cutout(scans_hist, odoms_hist, len(scan),
                 nsamp=drow.N_SAMP, laserIncrement=laser_inc)
    confs, votes = drow.forward_one(cut)   # (N_beams, 4), (N_beams, 2)

    angles = angles_fn(len(scan))
    r_new, phi_new = _win2global(
        scan[None], angles[None],
        votes[None, :, 0], votes[None, :, 1],
    )
    x_votes = r_new * -np.sin(phi_new)
    y_votes = r_new *  np.cos(phi_new)

    dets = votes_to_detections(x_votes, y_votes, confs[None], **_RESULT_CONF)
    return dets[0]   # list of (x, y, probs) for this frame


def run_algo_frame(algo: AlgorithmicDetector, dataset, seq: int, det: int):
    """
    Run AlgorithmicDetector on a single annotated frame.

    Returns np.ndarray of shape (N, 2) with (x, y) positions.
    """
    iscan = dataset.idet2iscan[seq][det]
    scans_hist, odoms_hist = dataset.get_scan(seq, iscan, algo.time_frame)
    result = np.array(algo.forward_one(scans_hist[-1], odoms_hist[-1]["xya"]))
    if result.ndim == 1:
        result = result.reshape(-1, 2)
    return result


# ---------------------------------------------------------------------------
# Pre-run everything (saves repeated forward passes when plotting many frames)
# ---------------------------------------------------------------------------

def run_all(dataset, cfg=None, use_drow: bool = True, verbose: bool = True):
    """
    Returns:
      drow_dets[seq][det] = list of (x, y, probs)   or None if use_drow=False
      algo_dets[seq][det] = np.ndarray (N, 2)
    """
    drow_dets = None

    # Algorithmic: fresh instance per sequence (stateful)
    if verbose:
        print("Running AlgorithmicDetector …")
    algo_dets = []
    for seq in range(len(dataset.det_id)):
        algo = AlgorithmicDetector(verbose=False)
        seq_algo = []
        for det in range(len(dataset.det_id[seq])):
            seq_algo.append(run_algo_frame(algo, dataset, seq, det))
        algo_dets.append(seq_algo)
        if verbose:
            print(f"  Sequence {seq}: {len(seq_algo)} frames")

    if use_drow:
        if verbose:
            print("\nLoading DROW detector …")
        drow = DrowDetector.init(verbose=verbose)

        if verbose:
            print("Running DROW forward pass …")
        drow_dets = []
        for seq in range(len(dataset.det_id)):
            seq_drow = []
            for det in range(len(dataset.det_id[seq])):
                seq_drow.append(run_drow_frame(drow, dataset, seq, det, cfg=cfg))
            drow_dets.append(seq_drow)
            if verbose:
                print(f"  Sequence {seq} done")

    return drow_dets, algo_dets


# ---------------------------------------------------------------------------
# Agreement statistics
# ---------------------------------------------------------------------------

def agreement_stats(dataset, drow_dets, algo_dets, eval_r: float = 0.5):
    """
    Count how often DROW and algorithmic detections agree (both fire within
    eval_r of each other), disagree, or one is silent.

    Also count how often each detector agrees with GT annotations.
    """
    rows = []
    for seq in range(len(dataset.det_id)):
        for det in range(len(dataset.det_id[seq])):
            all_ann = (dataset.det_wc[seq][det]
                       + dataset.det_wa[seq][det]
                       + dataset.det_wp[seq][det])
            gt_xy = np.array([rphi_to_xy(r, phi)
                               for r, phi in all_ann]) if all_ann else np.empty((0, 2))

            algo = algo_dets[seq][det]
            drow = (np.array([[d[0], d[1]] for d in drow_dets[seq][det]])
                    if drow_dets and drow_dets[seq][det]
                    else np.empty((0, 2)))

            def _near(src, tgt, r):
                if len(src) == 0 or len(tgt) == 0:
                    return 0
                from scipy.spatial.distance import cdist
                return int((cdist(src, tgt).min(axis=1) < r).sum())

            rows.append({
                "seq": seq, "det": det,
                "n_gt":   len(gt_xy),
                "n_algo": len(algo),
                "n_drow": len(drow),
                "algo_near_drow": _near(algo, drow, eval_r),
                "drow_near_algo": _near(drow, algo, eval_r),
                "algo_near_gt":   _near(algo, gt_xy, eval_r),
                "drow_near_gt":   _near(drow, gt_xy, eval_r),
                "gt_near_algo":   _near(gt_xy, algo, eval_r),
                "gt_near_drow":   _near(gt_xy, drow, eval_r),
            })
    return rows


def print_agreement(rows, eval_r: float = 0.5):
    n_frames      = len(rows)
    n_gt          = sum(r["n_gt"]   for r in rows)
    n_algo        = sum(r["n_algo"] for r in rows)
    n_drow        = sum(r["n_drow"] for r in rows)
    algo_drow_agr = sum(r["algo_near_drow"] for r in rows)
    drow_algo_agr = sum(r["drow_near_algo"] for r in rows)
    algo_gt       = sum(r["algo_near_gt"]   for r in rows)
    drow_gt       = sum(r["drow_near_gt"]   for r in rows)
    gt_algo       = sum(r["gt_near_algo"]   for r in rows)
    gt_drow       = sum(r["gt_near_drow"]   for r in rows)

    print()
    print(f"=== Detector agreement  (eval_r = {eval_r} m, {n_frames} frames) ===")
    print()
    print(f"  Total GT annotations   : {n_gt}")
    print(f"  Total algo detections  : {n_algo}")
    print(f"  Total DROW detections  : {n_drow}")
    print()
    print("  Inter-detector agreement:")
    _pct = lambda a, b: f"{a/b:.1%}" if b > 0 else "n/a"
    print(f"    Algo dets near a DROW det : {algo_drow_agr} / {n_algo}  ({_pct(algo_drow_agr, n_algo)})")
    print(f"    DROW dets near an algo det: {drow_algo_agr} / {n_drow}  ({_pct(drow_algo_agr, n_drow)})")
    print()
    print("  Each detector vs GT annotations:")
    print(f"    Algo dets near GT         : {algo_gt} / {n_algo}  ({_pct(algo_gt, n_algo)})")
    print(f"    DROW dets near GT         : {drow_gt} / {n_drow}  ({_pct(drow_gt, n_drow)})")
    print(f"    GT labels near algo det   : {gt_algo} / {n_gt}   ({_pct(gt_algo, n_gt)})")
    print(f"    GT labels near DROW det   : {gt_drow} / {n_gt}   ({_pct(gt_drow, n_gt)})")
    print()
    print("  How to read:")
    print("    High inter-detector agreement + low GT agreement")
    print("      → detectors agree on real people; GT labels are wrong")
    print("    High inter-detector agreement + high GT agreement")
    print("      → everything consistent; parsing is correct")
    print("    Low inter-detector agreement")
    print("      → detectors disagree; inspect individual frames")
    print()


# ---------------------------------------------------------------------------
# Visualisation
# ---------------------------------------------------------------------------

def plot_frame(dataset, drow_dets, algo_dets, seq: int, det: int, cfg=None):
    angles_fn = cfg.angles_fn if cfg else laser_angles
    fov_min   = cfg.fov_min   if cfg else laser_minimum
    fov_max   = cfg.fov_max   if cfg else laser_maximum

    iscan  = dataset.idet2iscan[seq][det]
    scan   = dataset.scans[seq][iscan]
    angles = angles_fn(len(scan))
    scan_x = scan * -np.sin(angles)
    scan_y = scan *  np.cos(angles)

    wc_x, wc_y = _ann_to_xy(dataset.det_wc[seq][det])
    wa_x, wa_y = _ann_to_xy(dataset.det_wa[seq][det])
    wp_x, wp_y = _ann_to_xy(dataset.det_wp[seq][det])

    algo = algo_dets[seq][det]
    drow = drow_dets[seq][det] if drow_dets else []

    fig, ax = plt.subplots(figsize=(10, 10))
    _draw_fov_blind_spot(ax, fov_min, fov_max)
    ax.scatter(scan_x, scan_y, s=4, c="steelblue", zorder=2, label="LiDAR points")

    # GT annotations — distinguish in-FoV from out-of-FoV
    for label, anns, colour in [
            ("wc", dataset.det_wc[seq][det], "orange"),
            ("wa", dataset.det_wa[seq][det], "purple"),
            ("wp", dataset.det_wp[seq][det], "red"),
    ]:
        in_fov, out_fov = _split_fov(anns, fov_min, fov_max)
        if in_fov:
            xs, ys = zip(*[rphi_to_xy(r, p) for r, p in in_fov])
            ax.scatter(xs, ys, s=220, c=colour, marker="x",
                       linewidths=2.5, zorder=5, label=f"GT {label} (in FoV)")
        if out_fov:
            xs, ys = zip(*[rphi_to_xy(r, p) for r, p in out_fov])
            ax.scatter(xs, ys, s=220, c=colour, marker="x", alpha=0.3,
                       linewidths=1.2, zorder=5,
                       label=f"GT {label} (outside FoV — laser-invisible)")

    # Algorithmic detections
    if len(algo):
        ax.scatter(algo[:, 0], algo[:, 1],
                   s=280, c="limegreen", marker="^",
                   linewidths=1.5, zorder=6,
                   label=f"Algo ({len(algo)} det)")

    # DROW detections — circles scaled by agnostic confidence
    for d in drow:
        x_d, y_d, probs = d
        p_agn = float(np.sum(probs[1:]))
        circ = plt.Circle((x_d, y_d), radius=max(0.05, p_agn * 0.6),
                           color="crimson", fill=False, linewidth=2, zorder=6)
        ax.add_patch(circ)
    if drow:
        ax.scatter([], [], c="crimson", marker="o",
                   s=100, label=f"DROW ({len(drow)} det)")

    ax.scatter([0], [0], s=160, c="black", marker="o", zorder=7, label="robot")

    n_gt = sum(len(getattr(dataset, f"det_{c}")[seq][det])
               for c in ("wc", "wa", "wp"))
    ax.set_aspect("equal")
    ax.set_xlim(-8, 8)
    ax.set_ylim(-6, 10)
    ax.set_title(
        f"Sequence {seq}  |  detection frame {det}  |  scan index {iscan}\n"
        f"GT: {n_gt}  |  Algo: {len(algo)}  |  DROW: {len(drow)}  "
        f"|  shaded = laser blind spot"
    )
    ax.set_xlabel("x [m]  (left / right)")
    ax.set_ylabel("y [m]  (forward)")
    ax.legend(loc="upper right", fontsize=8)
    return fig


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Compare DROW and algorithmic detections frame by frame.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--dataset",    choices=["drow", "frog"], default="drow",
                        help="Dataset to use (default: drow)")
    parser.add_argument("--split",      choices=["test", "train", "val"], default="test",
                        help="FROG split (default: test; ignored for DROW)")
    parser.add_argument("--seq",        type=int,  default=1,
                        help="Sequence index to visualise (default: 1)")
    parser.add_argument("--det",        type=int,  default=3,
                        help="Detection index to visualise (default: 3)")
    parser.add_argument("--all-frames", action="store_true",
                        help="Save every frame of the chosen sequence")
    parser.add_argument("--all-seqs",   action="store_true",
                        help="Save frames for every sequence (implies --all-frames)")
    parser.add_argument("--stats",      action="store_true",
                        help="Print inter-detector agreement statistics")
    parser.add_argument("--no-drow",    action="store_true",
                        help="Skip DROW forward pass (algorithmic only, much faster)")
    parser.add_argument("--eval-r",     type=float, default=0.5,
                        help="Agreement radius in metres (default: 0.5)")
    parser.add_argument("--outdir",     type=Path,  default=Path("plots"),
                        help="Output directory for PNG files (default: plots/)")
    args = parser.parse_args()

    dataset, cfg = _setup(args)

    drow_dets, algo_dets = run_all(dataset, cfg=cfg,
                                   use_drow=not args.no_drow, verbose=True)

    if args.stats:
        rows = agreement_stats(dataset, drow_dets, algo_dets, eval_r=args.eval_r)
        print_agreement(rows, eval_r=args.eval_r)

    # --- Plots ---
    args.outdir.mkdir(parents=True, exist_ok=True)
    seqs_to_plot = range(len(dataset.scan_id)) if args.all_seqs else [args.seq]

    for seq in seqs_to_plot:
        if args.all_frames or args.all_seqs:
            dets_to_plot = range(len(dataset.det_id[seq]))
        else:
            dets_to_plot = [args.det]

        for det in dets_to_plot:
            if det >= len(dataset.det_id[seq]):
                print(f"  Sequence {seq} has only {len(dataset.det_id[seq])} frames, "
                      f"skipping det={det}")
                continue
            fig = plot_frame(dataset, drow_dets, algo_dets, seq, det, cfg=cfg)
            out = args.outdir / f"cmp_seq{seq}_det{det}.png"
            fig.savefig(out, dpi=150, bbox_inches="tight")
            plt.close(fig)
            print(f"Saved → {out}")


if __name__ == "__main__":
    main()
