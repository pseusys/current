#!/usr/bin/env python3
"""
DROW dataset verification script.

Answers two questions:
  1. Are the GT annotations correctly aligned with the laser scan data?
     (mis-alignment = parsing bug; random scatter = genuine mis-annotation)
  2. What AUC does the DROW detector achieve on the test set so we can compare
     it with the numbers reported in the original paper?

Usage
-----
  # Fast: only alignment stats + one scan plot per sequence
  python verify_dataset.py

  # Slow: also run detector and print AUC numbers
  python verify_dataset.py --auc

  # Plot a specific frame
  python verify_dataset.py --seq 0 --det 42

  # Save all per-sequence plots
  python verify_dataset.py --all-plots

Reference AUC numbers (original DROW paper, WNet3xLF2p T=5 odom=rot trainval):
  Person (wp)     agnostic  ~68-71 %   (eval radius 0.5 m)
  Wheelchair (wc)           ~80+ %
"""

import argparse
from pathlib import Path

import numpy as np
import matplotlib
matplotlib.rcParams["font.family"] = "DejaVu Sans"
import matplotlib.pyplot as plt
plt.style.use("ggplot")

from sklearn.metrics import auc

from follow_the_drow.datasets import DROW_Dataset
from follow_the_drow.utils.drow_utils import (
    rphi_to_xy, laser_angles, laser_minimum, laser_maximum,
    linearize, comp_prec_rec_softmax,
)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _safe_auc(recs, precs):
    """AUC with NaN filtering and sorted recall axis."""
    mask = ~np.isnan(recs) & ~np.isnan(precs)
    r, p = recs[mask], precs[mask]
    if len(r) < 2:
        return float("nan")
    order = np.argsort(r)
    return float(auc(r[order], p[order]))


def _ann_to_xy(anns):
    """Convert a list of (r, phi) annotation pairs to (xs, ys) arrays."""
    if len(anns) == 0:
        return np.array([]), np.array([])
    xy = np.array([rphi_to_xy(r, phi) for r, phi in anns])
    return xy[:, 0], xy[:, 1]


# ---------------------------------------------------------------------------
# 1. Visual alignment check
# ---------------------------------------------------------------------------

def plot_scan_with_annotations(dataset: DROW_Dataset, seq_idx: int, det_idx: int):
    """
    Plot one laser scan with its ground-truth annotations overlaid.

    If annotations land on visible point clusters the parsing is correct.
    If they are systematically offset (e.g. rotated 90°) there is a
    coordinate-convention bug.  If they appear random there is genuine
    mis-annotation in the dataset.
    """
    iscan = dataset.idet2iscan[seq_idx][det_idx]
    scan  = dataset.scans[seq_idx][iscan]
    angles = laser_angles(len(scan))

    # LiDAR points in Cartesian (same convention as rphi_to_xy)
    scan_x = scan * -np.sin(angles)
    scan_y = scan *  np.cos(angles)

    wc_x, wc_y = _ann_to_xy(dataset.det_wc[seq_idx][det_idx])
    wa_x, wa_y = _ann_to_xy(dataset.det_wa[seq_idx][det_idx])
    wp_x, wp_y = _ann_to_xy(dataset.det_wp[seq_idx][det_idx])

    fig, ax = plt.subplots(figsize=(9, 9))
    ax.scatter(scan_x, scan_y, s=4, c="steelblue", zorder=2, label="LiDAR points")
    if len(wc_x):
        ax.scatter(wc_x, wc_y, s=220, c="orange", marker="x",
                   linewidths=2.5, zorder=5, label="wheelchair (wc)")
    if len(wa_x):
        ax.scatter(wa_x, wa_y, s=220, c="purple", marker="x",
                   linewidths=2.5, zorder=5, label="walker (wa)")
    if len(wp_x):
        ax.scatter(wp_x, wp_y, s=220, c="red",    marker="x",
                   linewidths=2.5, zorder=5, label="person (wp)")
    ax.scatter([0], [0], s=180, c="black", marker="o", zorder=6, label="robot")

    total = len(wc_x) + len(wa_x) + len(wp_x)
    ax.set_aspect("equal")
    ax.set_title(
        f"Sequence {seq_idx}  |  detection frame {det_idx}  |  scan index {iscan}\n"
        f"{total} annotation(s):  {len(wc_x)} wc, {len(wa_x)} wa, {len(wp_x)} wp"
    )
    ax.set_xlabel("x [m]  (left / right)")
    ax.set_ylabel("y [m]  (forward)")
    ax.legend(loc="upper right")
    return fig


def _first_annotated(dataset: DROW_Dataset, seq_idx: int):
    """Return the first detection index in a sequence that has at least one label."""
    for i in range(len(dataset.det_id[seq_idx])):
        total = (len(dataset.det_wc[seq_idx][i])
                 + len(dataset.det_wa[seq_idx][i])
                 + len(dataset.det_wp[seq_idx][i]))
        if total > 0:
            return i
    return None


# ---------------------------------------------------------------------------
# 2. Quantitative alignment statistics
# ---------------------------------------------------------------------------

def fov_coverage_stats(dataset: DROW_Dataset) -> dict:
    """
    Count what fraction of annotations fall inside the laser's field of view.

    The Sick LMS covers ±112.25° (1.959 rad) centred on the robot's forward axis.
    Annotations whose |phi| > laser_maximum were made from camera data and point
    to a direction the laser physically cannot scan.  These annotations will always
    be counted as missed detections, silently hurting recall.

    This is a DATASET QUALITY issue (not a parsing bug): the DROW annotators used
    the RGB camera to track people even when they walked outside the laser's FoV.
    The original paper has the same issue — but knowing the fraction helps calibrate
    how much of the recall penalty is "fair" vs unavoidable.
    """
    results = {k: {"total": 0, "in_fov": 0} for k in ("wc", "wa", "wp")}
    for seq in range(len(dataset.det_id)):
        for det in range(len(dataset.det_id[seq])):
            for label, anns in [("wc", dataset.det_wc[seq][det]),
                                 ("wa", dataset.det_wa[seq][det]),
                                 ("wp", dataset.det_wp[seq][det])]:
                for r, phi in anns:
                    results[label]["total"] += 1
                    if laser_minimum <= phi <= laser_maximum:
                        results[label]["in_fov"] += 1
    return results


def print_fov_report(stats: dict):
    print()
    print("=== Annotation field-of-view coverage ===")
    print(f"  Laser FoV: {np.degrees(laser_minimum):.1f}° … {np.degrees(laser_maximum):.1f}°"
          f"  ({np.degrees(laser_maximum - laser_minimum):.1f}° total)")
    print()
    print(f"{'Class':<8} {'Total':>6}  {'In-FoV':>8}  {'Out-of-FoV':>12}")
    print("-" * 44)
    for label, s in stats.items():
        n, k = s["total"], s["in_fov"]
        if n == 0:
            print(f"{label:<8} {'0':>6}")
            continue
        print(f"{label:<8} {n:>6}  {k/n:>7.1%}  {(n-k)/n:>11.1%}")
    print()
    print("  Out-of-FoV annotations cannot be detected by the laser.")
    print("  They count as permanent missed detections and suppress recall.")
    print("  The original DROW paper has the same issue — both AUCs are comparable.")
    print()


def annotation_alignment_stats(dataset: DROW_Dataset) -> dict:
    """
    For every GT annotation compute the distance to the nearest LiDAR point.

    Interpretation:
      median < 0.15 m  →  annotations sit directly on scan clusters  (good)
      median 0.15–0.5 m →  small but consistent offset  (check coordinate signs)
      median > 0.5 m   →  severe mis-alignment; likely parsing bug OR bad labels
    """
    angles = laser_angles(dataset.scans[0].shape[-1])
    results = {"wc": [], "wa": [], "wp": []}

    for seq in range(len(dataset.det_id)):
        for det in range(len(dataset.det_id[seq])):
            iscan = dataset.idet2iscan[seq][det]
            scan  = dataset.scans[seq][iscan]

            scan_pts = np.stack([scan * -np.sin(angles),
                                 scan *  np.cos(angles)], axis=1)  # (N, 2)

            for label, anns in [("wc", dataset.det_wc[seq][det]),
                                 ("wa", dataset.det_wa[seq][det]),
                                 ("wp", dataset.det_wp[seq][det])]:
                for r, phi in anns:
                    gx, gy  = rphi_to_xy(r, phi)
                    ann_pt  = np.array([[gx, gy]])
                    min_dist = float(np.min(np.linalg.norm(scan_pts - ann_pt, axis=1)))
                    results[label].append(min_dist)

    return results


def print_alignment_report(stats: dict):
    print()
    print("=== Annotation → nearest LiDAR point distance ===")
    print(f"{'Class':<8} {'Count':>6}  {'Mean':>7}  {'Median':>7}  {'<0.3 m':>7}  {'<0.5 m':>7}")
    print("-" * 56)
    for label, dists in stats.items():
        if not dists:
            print(f"{label:<8} {'0':>6}")
            continue
        d = np.array(dists)
        print(f"{label:<8} {len(d):>6}  {d.mean():>6.3f}m  "
              f"{np.median(d):>6.3f}m  "
              f"{(d < 0.3).mean():>6.1%}  "
              f"{(d < 0.5).mean():>6.1%}")
    print()
    print("  >80 % within 0.5 m  →  annotations correctly placed")
    print("  <30 % within 0.5 m  →  systematic mis-alignment (bug) or bad labels")
    print()


# ---------------------------------------------------------------------------
# 3. AUC computation
# ---------------------------------------------------------------------------

# Hyperparameters from the original notebook (kept identical so results are
# directly comparable to what was reported in the paper repository).
_RESULT_CONF = {
    "blur_sigma":          2.23409276092903,
    "blur_win":            11,
    "bin_size":            0.04566379529562327,
    "vote_collect_radius": 0.6351825665330302,
    "min_thresh":          0.0027015322261551397,
    "class_weights":       [0.89740097838073, 0.3280190481521334, 0.4575675717820713],
}


def compute_auc(dataset: DROW_Dataset, eval_r: float = 0.5):
    """
    Run the DROW detector on the full dataset and return per-class AUC.

    eval_r : evaluation radius in metres (paper default = 0.5 m).
    """
    from follow_the_drow.detectors import DrowDetector

    print("Loading DROW detector weights …")
    net = DrowDetector.init(time_frame_size=dataset.time_frame)
    print(f"  {net.parameters_number:,} parameters")

    seqs, scans, wcs, was, wps = linearize(
        dataset.scan_id, dataset.scans,
        dataset.det_id, dataset.det_wc, dataset.det_wa, dataset.det_wp,
    )

    print("Running forward pass on test set (this takes ~1 s / frame without GPU) …")
    confs, offs = net.forward_all(dataset)

    conf = {**_RESULT_CONF, "eval_r": eval_r}
    wd, wc, wa, wp = comp_prec_rec_softmax(scans, wcs, was, wps, confs, offs, **conf)
    return wd, wc, wa, wp


def print_auc_report(wd, wc, wa, wp, eval_r: float = 0.5):
    print()
    print(f"=== DROW detector AUC  (eval radius = {eval_r} m) ===")
    print(f"  Agnostic (any):  {_safe_auc(*wd[:2]):.1%}")
    print(f"  Wheelchair (wc): {_safe_auc(*wc[:2]):.1%}")
    print(f"  Walker (wa):     {_safe_auc(*wa[:2]):.1%}")
    print(f"  Person (wp):     {_safe_auc(*wp[:2]):.1%}")
    print()
    print("  Original paper (WNet3xLF2p, T=5, odom=rot, trained on trainval):")
    print("    Person (wp) agnostic  ~68–71 %   at eval_r = 0.5 m")
    print("    Wheelchair (wc)       ~80+ %")
    print()
    print("  How to interpret:")
    print("    ~ paper numbers  →  dataset parsed correctly; any accuracy gap")
    print("                        is due to annotation quality, not code bugs")
    print("    wp ~ 0 %         →  coordinate system / eval-radius mismatch")
    print("    very different   →  suspect a parsing regression")
    print()


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Verify DROW dataset parsing and optionally compute AUC.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--seq",       type=int,  default=0,
                        help="Sequence index for the scan plot (default: 0)")
    parser.add_argument("--det",       type=int,  default=None,
                        help="Detection index to plot (default: first annotated frame)")
    parser.add_argument("--all-plots", action="store_true",
                        help="Save one plot per sequence instead of just --seq")
    parser.add_argument("--auc",       action="store_true",
                        help="Run the DROW detector and print AUC numbers "
                             "(slow — ~1 s/frame on CPU)")
    parser.add_argument("--eval-r",    type=float, default=0.5,
                        help="Evaluation radius in metres for AUC (default: 0.5)")
    parser.add_argument("--outdir",    type=Path,  default=Path("."),
                        help="Directory for saved plot PNGs (default: current dir)")
    args = parser.parse_args()

    # --- Load dataset ---
    print("Loading DROW test set …")
    dataset = DROW_Dataset()
    print(f"  {len(dataset.scan_id)} sequence(s) loaded\n")

    # --- FoV coverage (fast, explains "random" annotations outside the scan) ---
    print("Checking annotation field-of-view coverage …")
    fov_stats = fov_coverage_stats(dataset)
    print_fov_report(fov_stats)

    # --- Alignment stats (fast, no detector needed) ---
    print("Computing annotation alignment statistics …")
    stats = annotation_alignment_stats(dataset)
    print_alignment_report(stats)

    # --- Scan plots ---
    args.outdir.mkdir(parents=True, exist_ok=True)
    seqs_to_plot = range(len(dataset.scan_id)) if args.all_plots else [args.seq]

    for seq in seqs_to_plot:
        det = args.det
        if det is None:
            det = _first_annotated(dataset, seq)
        if det is None:
            print(f"Sequence {seq}: no annotated frames found, skipping plot.")
            continue

        fig = plot_scan_with_annotations(dataset, seq, det)
        out = args.outdir / f"scan_seq{seq}_det{det}.png"
        fig.savefig(out, dpi=150, bbox_inches="tight")
        plt.close(fig)
        print(f"Saved plot → {out}")

    # --- AUC (optional, slow) ---
    if args.auc:
        print()
        wd, wc, wa, wp = compute_auc(dataset, eval_r=args.eval_r)
        print_auc_report(wd, wc, wa, wp, eval_r=args.eval_r)


if __name__ == "__main__":
    main()
