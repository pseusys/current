#!/usr/bin/env python3
"""
Throughput benchmark for all person detectors.

Measures, for each model × device × mode:
  - Preprocessing : cutout  vs  aligned_scan_xyz  (always CPU)
  - Eval mode     : forward pass only  (torch.no_grad)
  - Train mode    : forward + backward + optimizer step

Devices tested automatically: CPU, DirectML (if torch-directml is installed).

Usage
-----
  python benchmark.py                          # defaults: N_beams=450, T=5
  python benchmark.py --n-beams 720            # FROG scanner config
  python benchmark.py --warmup 10 --iters 30  # more stable timing
  python benchmark.py --no-dml                # skip DirectML even if available
"""

import argparse
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Tuple

import numpy as np
import torch
import torch.nn.functional as F

_HERE = Path(__file__).parent
sys.path.insert(0, str(_HERE.parent / "library"))
sys.path.insert(0, str(_HERE))

from follow_the_drow.detectors import (
    DrowDetector,
    DrSpaamDetector,
    FullScanCNNDetector,
    SpaceTimeCNNDetector,
    FullScanTransformerDetector,
)
from follow_the_drow.detectors.architectures import PersonDetector
from follow_the_drow.utils.drow_utils import (
    cutout as _cutout,
    aligned_scan_xyz as _aligned_scan_xyz,
    laser_angles,
)
from train import _make_optimizer


# ──────────────────────────────────────────────────────────────────────────────
# Model registry
# ──────────────────────────────────────────────────────────────────────────────

def _build_models(T: int) -> List[Tuple[str, torch.nn.Module, str]]:
    """Return list of (name, model, input_mode)."""
    return [
        ("DrowDetector",
            DrowDetector(dropout=0.5, time_frame_size=T, verbose=False),
            "cutout"),
        ("DrSpaamDetector",
            DrSpaamDetector(n_time=T),
            "cutout"),
        ("PersonDet / attn_sum",
            PersonDetector(arch="attn_sum", n_time=T),
            "cutout"),
        ("PersonDet / attn_sum + BeamConv",
            PersonDetector(arch="attn_sum", n_time=T, beam_attn=True),
            "cutout"),
        ("PersonDet / gru",
            PersonDetector(arch="gru", n_time=T),
            "cutout"),
        ("FullScanCNN",
            FullScanCNNDetector(n_time=T),
            "full_scan"),
        ("SpaceTimeCNN",
            SpaceTimeCNNDetector(n_time=T),
            "full_scan"),
        ("FullScanTransformer",
            FullScanTransformerDetector(n_time=T),
            "full_scan"),
    ]


# ──────────────────────────────────────────────────────────────────────────────
# Synthetic data helpers
# ──────────────────────────────────────────────────────────────────────────────

def _fake_scans(n_beams: int, T: int):
    """Return (scans (T,N), odoms list) for preprocessing benchmarks."""
    scans = np.random.uniform(0.5, 10.0, (T, n_beams)).astype(np.float32)
    odoms = [{"xya": np.array([0.0, 0.0, i * 0.01], dtype=np.float32)}
             for i in range(T)]
    return scans, odoms


def _fake_input(input_mode: str, n_beams: int, T: int, device) -> torch.Tensor:
    if input_mode == "cutout":
        return torch.randn(n_beams, T, PersonDetector.N_SAMP, device=device)
    else:
        return torch.randn(n_beams, T, 3, device=device)


def _fake_targets(n_beams: int, device):
    labels    = torch.randint(0, 4, (n_beams,), device=device)
    vote_tgts = torch.randn(n_beams, 2, device=device)
    return labels, vote_tgts


# ──────────────────────────────────────────────────────────────────────────────
# Timing helper
# ──────────────────────────────────────────────────────────────────────────────

def _time_fn(fn, warmup: int, iters: int) -> Tuple[float, float]:
    """Run fn() warmup+iters times; return (mean_ms, std_ms) over timed iters."""
    for _ in range(warmup):
        fn()
    times = []
    for _ in range(iters):
        t0 = time.perf_counter()
        fn()
        times.append((time.perf_counter() - t0) * 1e3)
    arr = np.array(times[2:])          # drop first 2 in case of JIT warm-up
    return float(arr.mean()), float(arr.std())


# ──────────────────────────────────────────────────────────────────────────────
# Preprocessing benchmark (CPU only — preprocessing always runs on CPU)
# ──────────────────────────────────────────────────────────────────────────────

def bench_preprocessing(n_beams: int, T: int,
                        warmup: int, iters: int) -> Dict[str, Tuple[float, float]]:
    scans, odoms = _fake_scans(n_beams, T)
    angles = laser_angles(n_beams)

    c_mean, c_std = _time_fn(
        lambda: _cutout(scans, odoms, n_beams, nsamp=PersonDetector.N_SAMP),
        warmup, iters,
    )
    a_mean, a_std = _time_fn(
        lambda: _aligned_scan_xyz(scans, odoms, angles),
        warmup, iters,
    )
    return {
        "cutout":           (c_mean, c_std),
        "aligned_scan_xyz": (a_mean, a_std),
    }


# ──────────────────────────────────────────────────────────────────────────────
# Model benchmark
# ──────────────────────────────────────────────────────────────────────────────

def bench_model(
    model: torch.nn.Module,
    input_mode: str,
    device,
    using_dml: bool,
    n_beams: int,
    T: int,
    warmup: int,
    iters: int,
) -> Dict[str, Tuple[float, float]]:
    """
    Benchmark one model on one device.

    Returns dict with keys 'eval' and 'train', each (mean_ms, std_ms).
    Timing includes:
      eval  — forward only  (torch.no_grad)
      train — forward + backward + optimizer step
    """
    model = model.to(device)
    x = _fake_input(input_mode, n_beams, T, device)
    labels, vote_tgts = _fake_targets(n_beams, device)
    opt = _make_optimizer(model.parameters(), lr=1e-3,
                          weight_decay=1e-4, using_dml=using_dml)

    # ── Eval ──────────────────────────────────────────────────────────────────
    model.eval()

    def run_eval():
        with torch.no_grad():
            logits, _ = model(x)
        # Force synchronisation: read one scalar back to CPU so DML completes.
        _ = logits.reshape(-1)[0].item()

    e_mean, e_std = _time_fn(run_eval, warmup, iters)

    # ── Train ─────────────────────────────────────────────────────────────────
    model.train()

    def run_train():
        logits, vpred = model(x)
        pos = labels > 0
        lv  = (F.mse_loss(vpred[pos], vote_tgts[pos])
               if pos.any() else vpred.new_tensor(0.0))
        loss = F.cross_entropy(logits, labels) + 0.02 * lv
        opt.zero_grad(set_to_none=True)
        loss.backward()
        opt.step()
        _ = loss.item()   # force sync

    t_mean, t_std = _time_fn(run_train, warmup, iters)

    return {"eval": (e_mean, e_std), "train": (t_mean, t_std)}


# ──────────────────────────────────────────────────────────────────────────────
# Formatting helpers
# ──────────────────────────────────────────────────────────────────────────────

def _ms(mean: float, std: float) -> str:
    return f"{mean:6.1f} ±{std:4.1f} ms"


def _fps(mean_ms: float, n_beams: int) -> str:
    """Scans/second (each forward covers one full scan = n_beams beams)."""
    if mean_ms <= 0:
        return "  n/a  "
    return f"{1000.0 / mean_ms:6.0f} sc/s"


# ──────────────────────────────────────────────────────────────────────────────
# Main
# ──────────────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="Throughput benchmark for all person detectors.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--n-beams", type=int, default=450,
                        help="Beams per scan (450=DROW, 720=FROG; default: 450)")
    parser.add_argument("--time-frame", type=int, default=5,
                        help="Temporal window T (default: 5)")
    parser.add_argument("--warmup", type=int, default=5,
                        help="Warm-up iterations before timing (default: 5)")
    parser.add_argument("--iters", type=int, default=20,
                        help="Timed iterations per configuration (default: 20)")
    parser.add_argument("--no-dml", action="store_true",
                        help="Skip DirectML even if torch-directml is installed")
    args = parser.parse_args()

    N  = args.n_beams
    T  = args.time_frame
    WU = args.warmup
    IT = args.iters

    # ── Device detection ──────────────────────────────────────────────────────
    devices = [("CPU", "cpu", False)]

    if not args.no_dml:
        try:
            import torch_directml
            dml_dev = torch_directml.device()
            devices.append(("DirectML", dml_dev, True))
            print(f"DirectML device detected: {dml_dev}")
        except ImportError:
            print("torch-directml not installed — skipping DML benchmarks.")

    if torch.cuda.is_available():
        devices.append(("CUDA", "cuda", False))
        print(f"CUDA device: {torch.cuda.get_device_name(0)}")

    print(f"\nConfig: N_beams={N}, T={T}, N_samp={PersonDetector.N_SAMP}, "
          f"warmup={WU}, iters={IT}\n")

    # ── Preprocessing benchmark ───────────────────────────────────────────────
    print("=" * 72)
    print("  PREPROCESSING  (always on CPU)")
    print("=" * 72)
    prep = bench_preprocessing(N, T, WU, IT)
    col_w = 22
    for name, (m, s) in prep.items():
        bar_len = max(1, int(m / 0.5))          # 1 char ≈ 0.5 ms
        bar = "#" * min(bar_len, 50)
        print(f"  {name:<{col_w}} {_ms(m, s)}   {_fps(m, N)}   {bar}")
    print()

    # ── Model benchmarks ──────────────────────────────────────────────────────
    models = _build_models(T)

    # Build header
    dev_names = [d[0] for d in devices]
    col_name  = 32
    col_cell  = 18
    header    = f"  {'Model':<{col_name}}"
    for dn in dev_names:
        header += f"  {'eval':>{col_cell // 2}}{'train':>{col_cell // 2}}"
    sep = "=" * (col_name + 4 + len(dev_names) * (col_cell + 2))

    for mode_label, mode_key in [("EVAL  (no_grad forward)", "eval"),
                                  ("TRAIN (fwd + bwd + opt.step)", "train")]:
        print("=" * 72)
        print(f"  {mode_label}")
        print("=" * 72)
        print(f"  {'Model':<{col_name}}", end="")
        for dn in dev_names:
            print(f"  {dn:^{col_cell}}", end="")
        print()
        print("  " + "-" * (col_name + len(dev_names) * (col_cell + 2)))

        for name, model, input_mode in models:
            row = f"  {name:<{col_name}}"
            for dev_name, dev, using_dml in devices:
                try:
                    result = bench_model(
                        model, input_mode, dev, using_dml,
                        N, T, WU, IT,
                    )
                    m, s = result[mode_key]
                    cell = f"{m:5.1f}±{s:4.1f}ms"
                    row += f"  {cell:^{col_cell}}"
                except Exception as exc:
                    row += f"  {'ERROR':^{col_cell}}"
                    print(f"\n  [WARN] {name} on {dev_name}: {exc}", end="")

            print(row)

        print()

    # ── Summary: speedup table ────────────────────────────────────────────────
    if len(devices) > 1:
        print("=" * 72)
        print("  SPEEDUP  (CPU eval time / device eval time)")
        print("=" * 72)
        print(f"  {'Model':<{col_name}}", end="")
        for dn in dev_names[1:]:
            print(f"  {dn:^{col_cell}}", end="")
        print()
        print("  " + "-" * (col_name + (len(dev_names) - 1) * (col_cell + 2)))

        for name, model, input_mode in models:
            row = f"  {name:<{col_name}}"
            cpu_eval = None
            per_device = {}
            for dev_name, dev, using_dml in devices:
                try:
                    result = bench_model(
                        model, input_mode, dev, using_dml,
                        N, T, WU, IT,
                    )
                    per_device[dev_name] = result["eval"][0]
                    if dev_name == "CPU":
                        cpu_eval = result["eval"][0]
                except Exception:
                    per_device[dev_name] = None

            for dn in dev_names[1:]:
                d_eval = per_device.get(dn)
                if cpu_eval and d_eval and d_eval > 0:
                    speedup = cpu_eval / d_eval
                    row += f"  {speedup:^{col_cell}.2f}×"
                else:
                    row += f"  {'n/a':^{col_cell}}"
            print(row)

        print()


if __name__ == "__main__":
    main()
