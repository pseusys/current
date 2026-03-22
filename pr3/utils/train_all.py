#!/usr/bin/env python3
"""
Train all learnable detectors sequentially.

Calls train_model() from train.py for each detector and collects the final
val-loss and AUC results in a summary table.

GRU-based models (fullscan_cnn, fullscan_transformer) are automatically run on
CPU even when DirectML is active, because aten::_thnn_fused_gru_cell is not
supported on that backend.

Usage
-----
  # All models, FROG, 30 epochs, early stopping with patience 5
  python train_all.py

  # Override common settings
  python train_all.py --epochs 50 --patience 10 --dataset drow --out-dir runs/

  # Quick smoke-test (5 % of data)
  python train_all.py --epochs 3 --subsample 0.05

  # Skip specific models
  python train_all.py --skip drow fullscan_cnn
"""

import argparse
import sys
import time
from pathlib import Path

# train.py is in the same directory — add it to the path if needed
sys.path.insert(0, str(Path(__file__).parent))
from train import (  # noqa: E402
    _default_args, train_model,
    _setup_datasets, _build_model, load_checkpoint,
    evaluate_auc, detect_device,
)

# Models that cannot run on DirectML (GRU kernel missing)
_GRU_MODELS = {"fullscan_cnn", "fullscan_transformer"}

# Canonical training order
_ALL_DETECTORS = [
    "drow",
    "drspaam",
    "fullscan_cnn",
    "spacetime_cnn",
    "fullscan_transformer",
]


def _parse() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Train all detectors sequentially.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    p.add_argument("--dataset",      choices=["drow", "frog"], default="frog")
    p.add_argument("--train-split",  default="train")
    p.add_argument("--val-split",    default="val")
    p.add_argument("--test-split",   default="test",
                   help="Split used for final test evaluation (default: test; '' to skip)")
    p.add_argument("--epochs",       type=int,   default=30)
    p.add_argument("--patience",     type=int,   default=5,
                   help="Early-stopping patience in epochs; 0=disabled (default: 5)")
    p.add_argument("--lr",           type=float, default=1e-3)
    p.add_argument("--weight-decay", type=float, default=1e-4)
    p.add_argument("--dropout",      type=float, default=0.5)
    p.add_argument("--lr-schedule",  default="plateau",
                   choices=["none", "cosine", "plateau"])
    p.add_argument("--subsample",    type=float, default=1.0)
    p.add_argument("--batch-size",   type=int,   default=4)
    p.add_argument("--auc-every",    type=int,   default=5,
                   help="Compute AUC every N epochs during training (default: 5)")
    p.add_argument("--out-dir",      type=Path,  default=Path("checkpoints"),
                   help="Directory for per-model checkpoint files (default: checkpoints/)")
    p.add_argument("--skip",         nargs="*",  default=[],
                   metavar="DETECTOR",
                   choices=_ALL_DETECTORS,
                   help="Detectors to skip")
    return p.parse_args()


def _fmt(val, fmt=".4f"):
    return f"{val:{fmt}}" if val is not None else "  n/a  "


def main():
    cli = _parse()
    cli.out_dir.mkdir(parents=True, exist_ok=True)

    skip = set(cli.skip)
    todo = [d for d in _ALL_DETECTORS if d not in skip]

    print(f"\n{'='*60}")
    print(f"  Training {len(todo)} model(s) on {cli.dataset.upper()} "
          f"— {cli.epochs} epochs, patience={cli.patience}")
    print(f"  Models : {', '.join(todo)}")
    print(f"  Output : {cli.out_dir.resolve()}")
    print(f"{'='*60}\n")

    test_split = cli.test_split.strip() or None

    summaries = []   # list of (detector, final_val_loss, best_auc_agnostic, elapsed_s)

    for det in todo:
        print(f"\n{'='*60}")
        print(f"  [{todo.index(det)+1}/{len(todo)}]  {det}")
        print(f"{'='*60}\n")

        args = _default_args(
            detector=det,
            dataset=cli.dataset,
            train_split=cli.train_split,
            val_split=cli.val_split,
            epochs=cli.epochs,
            patience=cli.patience,
            lr=cli.lr,
            weight_decay=cli.weight_decay,
            dropout=cli.dropout,
            lr_schedule=cli.lr_schedule,
            subsample=cli.subsample,
            batch_size=cli.batch_size,
            auc_every=cli.auc_every,
            out=cli.out_dir / f"{det}.pth",
            # GRU models must run on CPU when DirectML is active
            force_cpu=(det in _GRU_MODELS),
        )

        t0 = time.perf_counter()
        try:
            history = train_model(args)
        except Exception as exc:
            print(f"\n  [ERROR] {det} failed: {exc}\n")
            summaries.append((det, None, None, None))
            continue
        elapsed = time.perf_counter() - t0

        final_val  = history["val_loss"][-1]  if history["val_loss"]  else None
        best_auc   = max(history["val_auc_agnostic"], default=None)
        summaries.append((det, final_val, best_auc, elapsed))

    # ------------------------------------------------------------------
    # Test evaluation
    # ------------------------------------------------------------------
    test_aucs = {}   # det -> {"agnostic": float, "wc": float, "wa": float, "wp": float}

    if test_split:
        print(f"\n{'='*60}")
        print(f"  TEST EVALUATION  (split='{test_split}')")
        print(f"{'='*60}\n")

        device, _ = detect_device()

        test_args = _default_args(
            dataset=cli.dataset,
            train_split=cli.train_split,
            val_split=test_split,   # reuse val_split slot to load the test split
        )
        _, test_ds, cfg = _setup_datasets(test_args)

        if test_ds is None:
            print(f"  [WARN] No data found for split '{test_split}' — skipping test eval.\n")
        else:
            for det, vl, *_ in summaries:
                ckpt = cli.out_dir / f"{det}.pth"
                if not ckpt.exists():
                    print(f"  [{det}] checkpoint not found, skipping.")
                    test_aucs[det] = None
                    continue

                cpu = det in _GRU_MODELS
                dev = "cpu" if cpu else device
                model_args = _default_args(detector=det, force_cpu=cpu)
                net = _build_model(model_args).to(dev)
                load_checkpoint(ckpt, net)

                try:
                    aucs = evaluate_auc(net, test_ds, cfg, device=dev,
                                        batch_size=cli.batch_size)
                    test_aucs[det] = aucs
                    print(f"  {det:<28}  agnostic={aucs['agnostic']:.1%}"
                          f"  wc={aucs['wc']:.1%}"
                          f"  wa={aucs['wa']:.1%}"
                          f"  wp={aucs['wp']:.1%}")
                except Exception as exc:
                    print(f"  [{det}] eval failed: {exc}")
                    test_aucs[det] = None

    # ------------------------------------------------------------------
    # Summary table
    # ------------------------------------------------------------------
    print(f"\n\n{'='*60}")
    print("  SUMMARY")
    print(f"{'='*60}")
    hdr = f"  {'Model':<28}  {'Val loss':>9}  {'Val AUC':>8}  {'Test AUC':>9}  {'Time':>8}"
    print(hdr)
    print(f"  {'-'*28}  {'-'*9}  {'-'*8}  {'-'*9}  {'-'*8}")
    for det, vl, best_auc, elapsed in summaries:
        t_str  = f"{elapsed/60:.1f} min" if elapsed is not None else "  FAILED"
        va_str = f"{best_auc:.1%}"       if best_auc is not None   else "    n/a"
        v_str  = _fmt(vl)                if vl is not None         else "  FAILED"
        ta     = test_aucs.get(det)
        ta_str = f"{ta['agnostic']:.1%}" if ta is not None         else "    n/a"
        print(f"  {det:<28}  {v_str:>9}  {va_str:>8}  {ta_str:>9}  {t_str:>8}")
    print()


if __name__ == "__main__":
    main()
