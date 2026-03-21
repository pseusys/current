#!/usr/bin/env python3
"""
Algorithmic detector verification script.

Runs the classical leg-detection algorithm (AlgorithmicDetector) on the DROW
or FROG test set and visualises its detections alongside the GT annotations.

Because the GT annotations appear to be mis-placed (see verify_dataset.py and
the scan plots), this script lets you judge the algorithmic detector *visually*:
does it fire on the visible human-shaped clusters in the scan, regardless of
what the annotations say?

Usage
-----
  # DROW — plot one frame per sequence (first annotated frame)
  python verify_algorithmic.py

  # FROG — test set
  python verify_algorithmic.py --dataset frog

  # FROG — specific split
  python verify_algorithmic.py --dataset frog --split train

  # Plot a specific frame
  python verify_algorithmic.py --seq 1 --det 3

  # Save all frames for a sequence
  python verify_algorithmic.py --seq 1 --all-frames

  # Save all frames for all sequences
  python verify_algorithmic.py --all-frames --all-seqs

  # Print detection statistics (coverage vs GT)
  python verify_algorithmic.py --stats
"""

import argparse
from pathlib import Path
from types import SimpleNamespace

import numpy as np
import matplotlib
matplotlib.rcParams["font.family"] = "DejaVu Sans"
import matplotlib.pyplot as plt
plt.style.use("ggplot")

from follow_the_drow.datasets import DROW_Dataset
from follow_the_drow.detectors import AlgorithmicDetector
from follow_the_drow.utils.drow_utils import (
    laser_angles, laser_minimum, laser_maximum, rphi_to_xy,
)


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
        )
    else:
        print("Loading DROW test set …")
        dataset = DROW_Dataset()
        cfg = SimpleNamespace(
            name="drow",
            angles_fn=laser_angles,
            fov_min=laser_minimum,
            fov_max=laser_maximum,
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
    """Split annotations into (in-FoV, out-of-FoV) lists."""
    in_, out_ = [], []
    for r, phi in anns:
        (in_ if fov_min <= phi <= fov_max else out_).append((r, phi))
    return in_, out_


def _draw_fov_blind_spot(ax, fov_min, fov_max, r_max=12):
    """Shade the laser's blind spot (directions outside the FoV)."""
    theta = np.linspace(fov_max, 2 * np.pi + fov_min, 120)
    xs = np.concatenate([[0], r_max * -np.sin(theta), [0]])
    ys = np.concatenate([[0], r_max *  np.cos(theta), [0]])
    ax.fill(xs, ys, color="gray", alpha=0.13, zorder=0, label="laser blind spot")


def _first_annotated(dataset, seq_idx):
    for i in range(len(dataset.det_id[seq_idx])):
        total = (len(dataset.det_wc[seq_idx][i])
                 + len(dataset.det_wa[seq_idx][i])
                 + len(dataset.det_wp[seq_idx][i]))
        if total > 0:
            return i
    return None


# ---------------------------------------------------------------------------
# Run detector on full dataset, grouped by sequence
# ---------------------------------------------------------------------------

def run_algorithmic(dataset, verbose: bool = True):
    """
    Run AlgorithmicDetector on every annotated frame of the dataset.

    Returns a nested list:  algo_dets[seq][det] = np.ndarray shape (N, 2)
    with (x, y) Cartesian positions of detected people (empty if none found).

    The detector is *stateful* (frequency / uncertainty tables), so we
    instantiate a fresh detector for each sequence to avoid state bleed.
    """
    algo_dets = []
    for seq in range(len(dataset.det_id)):
        algo = AlgorithmicDetector(verbose=False)
        seq_dets = []
        for det in range(len(dataset.det_id[seq])):
            iscan = dataset.idet2iscan[seq][det]
            scans, odoms = dataset.get_scan(seq, iscan, algo.time_frame)
            result = np.array(algo.forward_one(scans[-1], odoms[-1]["xya"]))
            seq_dets.append(result)
        algo_dets.append(seq_dets)
        if verbose:
            print(f"  Sequence {seq}: {len(seq_dets)} frames processed")
    return algo_dets


# ---------------------------------------------------------------------------
# Detection statistics
# ---------------------------------------------------------------------------

def detection_stats(dataset, algo_dets, eval_r: float = 0.5):
    """
    For each annotated frame count:
      - N_gt   : number of GT annotations
      - N_det  : number of algorithmic detections
      - N_match: detections within eval_r of any GT annotation
      - N_fp   : detections that are NOT within eval_r of any GT annotation

    Returns a dict with aggregated counts and a per-frame list.
    """
    frames = []
    for seq in range(len(dataset.det_id)):
        for det in range(len(dataset.det_id[seq])):
            all_ann = (dataset.det_wc[seq][det]
                       + dataset.det_wa[seq][det]
                       + dataset.det_wp[seq][det])
            gt_xy = np.array([rphi_to_xy(r, phi) for r, phi in all_ann]) if all_ann else np.empty((0, 2))

            dets = algo_dets[seq][det]
            if dets.ndim == 1:
                dets = dets.reshape(-1, 2)

            n_gt  = len(gt_xy)
            n_det = len(dets)
            n_match = 0
            n_fp    = 0

            if n_det > 0 and n_gt > 0:
                from scipy.spatial.distance import cdist
                dist = cdist(dets, gt_xy)
                matched = np.min(dist, axis=1) < eval_r
                n_match = int(matched.sum())
                n_fp    = n_det - n_match
            elif n_det > 0:
                n_fp = n_det

            frames.append({"seq": seq, "det": det,
                            "n_gt": n_gt, "n_det": n_det,
                            "n_match": n_match, "n_fp": n_fp})

    total_gt    = sum(f["n_gt"]    for f in frames)
    total_det   = sum(f["n_det"]   for f in frames)
    total_match = sum(f["n_match"] for f in frames)
    total_fp    = sum(f["n_fp"]    for f in frames)

    recall_approx = total_match / total_gt  if total_gt  > 0 else float("nan")
    precision     = total_match / total_det if total_det > 0 else float("nan")

    return {
        "frames": frames,
        "total_gt":    total_gt,
        "total_det":   total_det,
        "total_match": total_match,
        "total_fp":    total_fp,
        "recall_approx": recall_approx,
        "precision":     precision,
    }


def print_stats(stats, eval_r: float = 0.5):
    print()
    print(f"=== Algorithmic detector vs GT annotations  (eval_r = {eval_r} m) ===")
    print(f"  Total GT annotations : {stats['total_gt']}")
    print(f"  Total detections     : {stats['total_det']}")
    print(f"  Detections near GT   : {stats['total_match']}")
    print(f"  False positives      : {stats['total_fp']}")
    print(f"  Approx. recall       : {stats['recall_approx']:.1%}")
    print(f"  Approx. precision    : {stats['precision']:.1%}")
    print()
    print("  NOTE: these numbers use the possibly-mis-annotated GT labels.")
    print("  Low recall may mean mis-annotation, not detector failure.")
    print("  Inspect individual frame plots to distinguish the two cases.")
    print()

    seqs = sorted(set(f["seq"] for f in stats["frames"]))
    print(f"  {'Seq':>4}  {'Frames':>7}  {'GT':>5}  {'Dets':>5}  "
          f"{'Match':>6}  {'FP':>5}  {'Recall':>7}  {'Prec':>7}")
    print("  " + "-" * 57)
    for s in seqs:
        sf = [f for f in stats["frames"] if f["seq"] == s]
        g  = sum(f["n_gt"]    for f in sf)
        d  = sum(f["n_det"]   for f in sf)
        m  = sum(f["n_match"] for f in sf)
        fp = sum(f["n_fp"]    for f in sf)
        r  = m / g if g > 0 else float("nan")
        p  = m / d if d > 0 else float("nan")
        print(f"  {s:>4}  {len(sf):>7}  {g:>5}  {d:>5}  "
              f"{m:>6}  {fp:>5}  {r:>6.1%}  {p:>6.1%}")
    print()


# ---------------------------------------------------------------------------
# Visualisation
# ---------------------------------------------------------------------------

def plot_frame(dataset, algo_dets, seq: int, det: int, cfg=None):
    """
    Plot one annotated frame with GT annotations and algorithmic detections.
    """
    angles_fn = cfg.angles_fn if cfg else laser_angles
    fov_min   = cfg.fov_min   if cfg else laser_minimum
    fov_max   = cfg.fov_max   if cfg else laser_maximum

    iscan = dataset.idet2iscan[seq][det]
    scan  = dataset.scans[seq][iscan]
    angles = angles_fn(len(scan))
    scan_x = scan * -np.sin(angles)
    scan_y = scan *  np.cos(angles)

    wc_x, wc_y = _ann_to_xy(dataset.det_wc[seq][det])
    wa_x, wa_y = _ann_to_xy(dataset.det_wa[seq][det])
    wp_x, wp_y = _ann_to_xy(dataset.det_wp[seq][det])

    dets = algo_dets[seq][det]
    if dets.ndim == 1:
        dets = dets.reshape(-1, 2)

    fig, ax = plt.subplots(figsize=(9, 9))
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
    if len(dets):
        ax.scatter(dets[:, 0], dets[:, 1],
                   s=300, c="limegreen", marker="^",
                   linewidths=1.5, zorder=6, label=f"Algo detections ({len(dets)})")

    ax.scatter([0], [0], s=160, c="black", marker="o", zorder=7, label="robot")

    n_gt = sum(len(getattr(dataset, f"det_{c}")[seq][det])
               for c in ("wc", "wa", "wp"))
    ax.set_aspect("equal")
    ax.set_xlim(-8, 8)
    ax.set_ylim(-6, 10)
    ax.set_title(
        f"Sequence {seq}  |  detection frame {det}  |  scan index {iscan}\n"
        f"GT: {n_gt} label(s)  |  Algo: {len(dets)} detection(s)  "
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
        description="Verify AlgorithmicDetector on DROW or FROG dataset.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--dataset",    choices=["drow", "frog"], default="drow",
                        help="Dataset to load (default: drow)")
    parser.add_argument("--split",      choices=["test", "train", "val"], default="test",
                        help="FROG split to load (default: test; ignored for DROW)")
    parser.add_argument("--seq",        type=int,  default=0,
                        help="Sequence index to visualise (default: 0)")
    parser.add_argument("--det",        type=int,  default=None,
                        help="Detection index to visualise (default: first annotated frame)")
    parser.add_argument("--all-frames", action="store_true",
                        help="Save every frame of the chosen sequence")
    parser.add_argument("--all-seqs",   action="store_true",
                        help="Save frames for every sequence (implies --all-frames)")
    parser.add_argument("--stats",      action="store_true",
                        help="Print detection statistics vs GT")
    parser.add_argument("--eval-r",     type=float, default=0.5,
                        help="Evaluation radius in metres (default: 0.5)")
    parser.add_argument("--outdir",     type=Path,  default=Path("plots"),
                        help="Directory for saved PNGs (default: plots/)")
    args = parser.parse_args()

    dataset, cfg = _setup(args)

    print("Running AlgorithmicDetector (fresh instance per sequence) …")
    algo_dets = run_algorithmic(dataset, verbose=True)
    print()

    if args.stats:
        stats = detection_stats(dataset, algo_dets, eval_r=args.eval_r)
        print_stats(stats, eval_r=args.eval_r)

    # --- Plots ---
    args.outdir.mkdir(parents=True, exist_ok=True)
    seqs_to_plot = range(len(dataset.scan_id)) if args.all_seqs else [args.seq]

    for seq in seqs_to_plot:
        if args.all_frames or args.all_seqs:
            dets_to_plot = range(len(dataset.det_id[seq]))
        else:
            d = args.det
            if d is None:
                d = _first_annotated(dataset, seq)
            dets_to_plot = [d] if d is not None else []

        for det in dets_to_plot:
            fig = plot_frame(dataset, algo_dets, seq, det, cfg=cfg)
            out = args.outdir / f"algo_seq{seq}_det{det}.png"
            fig.savefig(out, dpi=150, bbox_inches="tight")
            plt.close(fig)
            print(f"Saved → {out}")


if __name__ == "__main__":
    main()
