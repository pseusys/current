#!/usr/bin/env python3
"""
PyTorch Lightning training script for PersonDetector on DROW or FROG data.

Supports five temporal architectures built on a shared 1-D CNN spatial encoder:
  mlp         — flatten T×C → two linear layers
  tcn         — causal dilated temporal convolutions
  gru         — GRU, last hidden state
  lstm        — LSTM, last hidden state
  transformer — multi-head self-attention + mean pooling

Device priority
---------------
  1. CUDA / ROCm (AMD GPU in WSL2) — detected automatically, Lightning "gpu"
  2. DirectML (any DX12 GPU on Windows) — install torch-directml, Lightning "cpu"
     with manual tensor transfer inside training steps
  3. CPU fallback

Usage
-----
  # Train GRU on FROG, 20 epochs, save to weights_gru.pth
  python train_lightning.py --arch gru --dataset frog --epochs 20

  # Train Transformer on DROW, subsample 10 % for a quick check
  python train_lightning.py --arch transformer --dataset drow --subsample 0.1 --epochs 5

  # Resume from a Lightning checkpoint
  python train_lightning.py --arch gru --resume lightning_logs/version_0/checkpoints/last.ckpt

  # Evaluate only (no training)
  python train_lightning.py --arch gru --weights weights_gru.pth --eval-only

Weight export
-------------
Two files are saved at the end of training:
  <out>.pth          — raw state_dict (load with PersonDetector.load())
  <out>_lightning.ckpt — full Lightning checkpoint (resume with --resume)
"""

import argparse
import random
import sys
from pathlib import Path
from types import SimpleNamespace

import numpy as np
import torch
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader, Subset

# ── make the library importable without installing in editable mode ──────────
_HERE = Path(__file__).parent
sys.path.insert(0, str(_HERE.parent / "library"))
# ── reuse helpers from the plain training script ─────────────────────────────
sys.path.insert(0, str(_HERE))
from train import make_targets, compute_loss, _setup_train, evaluate_auc, _make_optimizer

import pytorch_lightning as pl
from pytorch_lightning.callbacks import ModelCheckpoint, TQDMProgressBar

from follow_the_drow.detectors.architectures import PersonDetector, ARCH_REGISTRY
from follow_the_drow.utils.drow_utils import cutout


# ---------------------------------------------------------------------------
# Device detection
# ---------------------------------------------------------------------------

def detect_device():
    """
    Return (device_str, accelerator, devices, using_dml).

    device_str  : str  — passed to tensor.to() when using_dml=True
    accelerator : str  — Lightning Trainer accelerator argument
    devices     : int  — Lightning Trainer devices argument
    using_dml   : bool — tensors must be moved to device_str manually in steps
    """
    if torch.cuda.is_available():
        n = torch.cuda.device_count()
        name = torch.cuda.get_device_name(0)
        print(f"CUDA / ROCm device: {name}  ({n} device(s))")
        return "cuda", "gpu", 1, False

    try:
        import torch_directml
        dml = torch_directml.device()
        print(f"DirectML device: {dml}")
        return str(dml), "cpu", "auto", True
    except ImportError:
        pass

    print("No GPU found — using CPU.")
    return "cpu", "cpu", "auto", False


# ---------------------------------------------------------------------------
# PyTorch Dataset
# ---------------------------------------------------------------------------

class LidarFrameDataset(Dataset):
    """
    One item = one annotated lidar frame.

    Returns a tuple of float32 / int64 tensors:
      cutout      : (N_beams, T, N_samp)
      labels      : (N_beams,)
      vote_targets: (N_beams, 2)
    """

    def __init__(self, dataset, cfg, time_frame: int, vote_radius: float = 0.6):
        self.dataset     = dataset
        self.cfg         = cfg
        self.time_frame  = time_frame
        self.vote_radius = vote_radius
        self.indices = [
            (seq, det_idx)
            for seq in range(len(dataset.det_id))
            for det_idx in range(len(dataset.det_id[seq]))
        ]

    def __len__(self) -> int:
        return len(self.indices)

    def __getitem__(self, idx: int):
        seq, det_idx = self.indices[idx]
        iscan = self.dataset.idet2iscan[seq][det_idx]
        scan  = self.dataset.scans[seq][iscan]
        scans_hist, odoms_hist = self.dataset.get_scan(seq, iscan, self.time_frame)

        angles = self.cfg.angles_fn(len(scan))
        gt_per_class = {
            1: self.dataset.det_wc[seq][det_idx],
            2: self.dataset.det_wa[seq][det_idx],
            3: self.dataset.det_wp[seq][det_idx],
        }
        labels, vote_targets = make_targets(scan, angles, gt_per_class,
                                            self.vote_radius)
        cut = cutout(scans_hist, odoms_hist, len(scan),
                     nsamp=PersonDetector.N_SAMP,
                     laserIncrement=self.cfg.laser_inc)

        return (
            torch.from_numpy(np.asarray(cut,          dtype=np.float32)),
            torch.from_numpy(labels),
            torch.from_numpy(vote_targets),
        )


# ---------------------------------------------------------------------------
# LightningDataModule
# ---------------------------------------------------------------------------

class LidarDataModule(pl.LightningDataModule):
    def __init__(self, args, cfg, train_ds, val_ds, pin_memory: bool = False):
        super().__init__()
        self.args       = args
        self.cfg        = cfg
        self.train_ds   = train_ds
        self.val_ds     = val_ds
        self.pin_memory = pin_memory
        self._train     = None
        self._val       = None

    def setup(self, stage=None):
        full_train = LidarFrameDataset(
            self.train_ds, self.cfg, self.args.time_frame, self.args.vote_radius
        )
        if self.args.subsample < 1.0:
            n = max(1, int(len(full_train) * self.args.subsample))
            idx = random.sample(range(len(full_train)), n)
            self._train = Subset(full_train, idx)
        else:
            self._train = full_train

        if self.val_ds is not None:
            full_val = LidarFrameDataset(
                self.val_ds, self.cfg, self.args.time_frame, self.args.vote_radius
            )
            if self.args.subsample < 1.0:
                n_val = max(1, int(len(full_val) * min(self.args.subsample * 2, 1.0)))
                idx_val = random.sample(range(len(full_val)), n_val)
                self._val = Subset(full_val, idx_val)
            else:
                self._val = full_val

    def train_dataloader(self):
        return DataLoader(
            self._train,
            batch_size=self.args.batch_size,
            shuffle=True,
            num_workers=self.args.num_workers,
            pin_memory=self.pin_memory,
        )

    def val_dataloader(self):
        if self._val is None:
            return []
        return DataLoader(
            self._val,
            batch_size=self.args.batch_size,
            shuffle=False,
            num_workers=self.args.num_workers,
            pin_memory=self.pin_memory,
        )


# ---------------------------------------------------------------------------
# LightningModule
# ---------------------------------------------------------------------------

class PersonDetectorModule(pl.LightningModule):
    """
    Lightning wrapper around PersonDetector.

    manual_device: if not None (DirectML case), tensors are moved to this
    device inside each step because Lightning doesn't know about DML.
    """

    def __init__(self, args, manual_device=None):
        super().__init__()
        self.args          = args
        self.manual_device = manual_device
        self.model = PersonDetector(
            arch=args.arch,
            n_time=args.time_frame,
            spatial_channels=args.spatial_channels,
            hidden=args.hidden,
            dropout=args.dropout,
            beam_attn=getattr(args, "beam_attn", False),
            beam_attn_type=getattr(args, "beam_attn_type", "conv"),
            beam_kernel=getattr(args, "beam_kernel", 11),
        )
        if manual_device is not None:
            self.model = self.model.to(manual_device)

    def _to_dev(self, *tensors):
        """Move tensors to the manual device (no-op when using CUDA/CPU)."""
        if self.manual_device is not None:
            return tuple(t.to(self.manual_device) for t in tensors)
        return tensors

    def forward(self, x):
        return self.model(x)

    def _shared_step(self, batch):
        x, labels, votes = batch
        x, labels, votes = self._to_dev(x, labels, votes)

        # Batch dimension: (BS, N_beams, T, S) → flatten beam dim into batch
        BS, N = x.shape[:2]
        x_flat      = x.reshape(BS * N, *x.shape[2:])
        labels_flat = labels.reshape(BS * N)
        votes_flat  = votes.reshape(BS * N, 2)

        logits, votes_pred = self.model(x_flat)
        loss, lc, lv = compute_loss(logits, votes_pred, labels_flat, votes_flat,
                                    self.args.vote_weight)
        return loss, lc, lv

    def training_step(self, batch, batch_idx):
        loss, lc, lv = self._shared_step(batch)
        self.log("train/loss",       loss.item(), on_step=False, on_epoch=True,
                 prog_bar=True,  sync_dist=False)
        self.log("train/class_loss", lc,          on_step=False, on_epoch=True)
        self.log("train/vote_loss",  lv,          on_step=False, on_epoch=True)
        return loss

    def validation_step(self, batch, batch_idx):
        loss, lc, lv = self._shared_step(batch)
        self.log("val/loss",       loss.item(), on_step=False, on_epoch=True,
                 prog_bar=True,  sync_dist=False)
        self.log("val/class_loss", lc,          on_step=False, on_epoch=True)
        self.log("val/vote_loss",  lv,          on_step=False, on_epoch=True)

    def configure_optimizers(self):
        using_dml = self.manual_device is not None
        return _make_optimizer(self.model.parameters(),
                               lr=self.args.lr,
                               weight_decay=self.args.weight_decay,
                               using_dml=using_dml)

    def save_weights(self, path: Path):
        """Save a deployment-ready checkpoint (load with PersonDetector.load())."""
        torch.save({
            "model":            self.model.state_dict(),
            "arch":             self.args.arch,
            "n_time":           self.args.time_frame,
            "spatial_channels": self.args.spatial_channels,
            "hidden":           self.args.hidden,
            "dropout":          self.args.dropout,
            "beam_attn":        getattr(self.args, "beam_attn", False),
            "beam_attn_type":   getattr(self.args, "beam_attn_type", "conv"),
            "beam_kernel":      getattr(self.args, "beam_kernel", 11),
            "dataset":          self.args.dataset,
        }, path)
        print(f"  Weights saved → {path}")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Train PersonDetector (modular arch) on DROW or FROG.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    # ── Architecture ──────────────────────────────────────────────────────────
    parser.add_argument("--arch", choices=list(ARCH_REGISTRY), default="attn_sum",
                        help="Temporal aggregator: 'attn_sum' (DR-SPAAM temporal, default) "
                             "or 'gru' (sequential, preferred with --beam-attn-type transformer)")
    parser.add_argument("--beam-attn", action="store_true",
                        help="Add beam-spatial attention before temporal agg "
                             "(+4 pp AUC; combine with --arch attn_sum for full DR-SPAAM)")
    parser.add_argument("--beam-attn-type", choices=["conv", "transformer"],
                        default="conv",
                        help="Beam attention type: 'conv'=local BeamNeighborConv (DR-SPAAM), "
                             "'transformer'=global BeamSelfAttention (default: conv)")
    parser.add_argument("--beam-kernel", type=int, default=11,
                        help="Kernel size for BeamNeighborConv (default: 11 ≈ ±5 beams, ignored for transformer)")
    parser.add_argument("--spatial-channels", type=int, default=256,
                        help="Output channels of the spatial encoder (default: 256)")
    parser.add_argument("--hidden", type=int, default=256,
                        help="Hidden size of the temporal head (default: 256)")
    parser.add_argument("--dropout", type=float, default=0.5,
                        help="Dropout in the spatial encoder (default: 0.5)")
    # ── Data ─────────────────────────────────────────────────────────────────
    parser.add_argument("--dataset", choices=["drow", "frog"], default="frog",
                        help="Dataset to train on (default: frog)")
    parser.add_argument("--train-split", default="train",
                        help="Training split name (default: train)")
    parser.add_argument("--val-split", default="val",
                        help="Validation split name (default: val; '' to disable)")
    parser.add_argument("--time-frame", type=int, default=5,
                        help="Number of scans in the time window (default: 5)")
    parser.add_argument("--vote-radius", type=float, default=0.6,
                        help="Positive beam radius around GT annotation (default: 0.6 m)")
    parser.add_argument("--subsample", type=float, default=1.0,
                        help="Fraction of frames per epoch (default: 1.0)")
    # ── Training ─────────────────────────────────────────────────────────────
    parser.add_argument("--epochs", type=int, default=10,
                        help="Number of training epochs (default: 10)")
    parser.add_argument("--lr", type=float, default=1e-3,
                        help="Adam learning rate (default: 1e-3)")
    parser.add_argument("--weight-decay", type=float, default=1e-4,
                        help="Adam weight decay (default: 1e-4)")
    parser.add_argument("--vote-weight", type=float, default=0.02,
                        help="Vote MSE loss weight (default: 0.02)")
    parser.add_argument("--batch-size", type=int, default=8,
                        help="Number of frames per batch (default: 8)")
    parser.add_argument("--num-workers", type=int, default=0,
                        help="DataLoader workers (default: 0; >0 may be slow on Windows)")
    # ── Checkpointing ────────────────────────────────────────────────────────
    parser.add_argument("--out", type=Path, default=Path("weights_person.pth"),
                        help="Output file for deployment weights (default: weights_person.pth)")
    parser.add_argument("--resume", type=Path, default=None,
                        help="Resume from a Lightning checkpoint (.ckpt)")
    parser.add_argument("--weights", type=Path, default=None,
                        help="Load a raw PersonDetector checkpoint for eval-only mode")
    parser.add_argument("--eval-only", action="store_true",
                        help="Skip training; evaluate AUC on the val (or train) split")
    parser.add_argument("--eval-r", type=float, default=0.5,
                        help="Evaluation radius for AUC (default: 0.5 m)")
    parser.add_argument("--auc-every", type=int, default=0,
                        help="Compute full AUC every N epochs (0 = only at end)")
    return parser


def main():
    args = build_parser().parse_args()
    args.val_split = args.val_split.strip() or None

    # ── Device ───────────────────────────────────────────────────────────────
    device_str, accelerator, devices, using_dml = detect_device()
    print()

    # ── Data ─────────────────────────────────────────────────────────────────
    train_ds, val_ds, cfg = _setup_train(args)

    # ── Module ───────────────────────────────────────────────────────────────
    manual_device = device_str if using_dml else None
    module = PersonDetectorModule(args, manual_device=manual_device)
    n_params = sum(p.numel() for p in module.model.parameters())
    print(f"Architecture: {args.arch}  |  {n_params:,} parameters\n")

    # ── Eval-only ─────────────────────────────────────────────────────────────
    if args.eval_only:
        if args.weights:
            module.model = PersonDetector.load(args.weights, map_location="cpu")
            if manual_device:
                module.model = module.model.to(manual_device)
        eval_ds = val_ds or train_ds
        print("=== Evaluation ===")
        infer_device = device_str if not using_dml else "cpu"
        aucs = _eval_auc(module.model, eval_ds, cfg, args, infer_device)
        _print_aucs(aucs)
        return

    # ── DataModule ────────────────────────────────────────────────────────────
    data = LidarDataModule(args, cfg, train_ds, val_ds,
                           pin_memory=(accelerator == "gpu"))

    # ── Callbacks ─────────────────────────────────────────────────────────────
    ckpt_cb = ModelCheckpoint(
        monitor="val/loss" if args.val_split else None,
        save_last=True,
        filename=f"{args.arch}-{{epoch:03d}}-{{val/loss:.4f}}",
    )
    callbacks = [ckpt_cb, TQDMProgressBar(refresh_rate=10)]

    # Optional per-epoch AUC callback
    if args.auc_every > 0:
        callbacks.append(_AucCallback(args, cfg, val_ds or train_ds,
                                      device_str, using_dml))

    # ── Trainer ───────────────────────────────────────────────────────────────
    trainer = pl.Trainer(
        max_epochs=args.epochs,
        accelerator=accelerator,
        devices=devices,
        callbacks=callbacks,
        enable_progress_bar=True,
        log_every_n_steps=1,
    )

    trainer.fit(
        module,
        datamodule=data,
        ckpt_path=str(args.resume) if args.resume else None,
    )

    # ── Final AUC ─────────────────────────────────────────────────────────────
    if val_ds is not None:
        print("\nComputing final AUC on val set …")
        infer_device = device_str if not using_dml else "cpu"
        aucs = _eval_auc(module.model, val_ds, cfg, args, infer_device)
        _print_aucs(aucs)

    # ── Save weights ──────────────────────────────────────────────────────────
    module.save_weights(args.out)
    lightning_ckpt = args.out.with_name(args.out.stem + "_lightning.ckpt")
    trainer.save_checkpoint(str(lightning_ckpt))
    print(f"  Lightning checkpoint → {lightning_ckpt}")


# ---------------------------------------------------------------------------
# AUC helpers
# ---------------------------------------------------------------------------

def _eval_auc(model, dataset, cfg, args, device: str) -> dict:
    """Run evaluate_auc from train.py against a PersonDetector.

    PersonDetector.forward() has the same signature as DrowDetector.forward()
    and N_SAMP is identical (48), so evaluate_auc works with no changes.
    """
    return evaluate_auc(model, dataset, cfg, eval_r=args.eval_r, device=device)


def _print_aucs(aucs: dict):
    print(f"  Agnostic (any) : {aucs['agnostic']:.1%}")
    print(f"  Wheelchair(wc) : {aucs['wc']:.1%}")
    print(f"  Walker    (wa) : {aucs['wa']:.1%}")
    print(f"  Person    (wp) : {aucs['wp']:.1%}")


class _AucCallback(pl.Callback):
    """Compute and log AUC every N epochs."""

    def __init__(self, args, cfg, dataset, device_str, using_dml):
        self.args       = args
        self.cfg        = cfg
        self.dataset    = dataset
        self.device_str = device_str
        self.using_dml  = using_dml

    def on_train_epoch_end(self, trainer, pl_module):
        epoch = trainer.current_epoch + 1
        if epoch % self.args.auc_every != 0:
            return
        infer_device = self.device_str if not self.using_dml else "cpu"
        aucs = _eval_auc(pl_module.model, self.dataset, self.cfg,
                         self.args, infer_device)
        for key, val in aucs.items():
            trainer.logger.log_metrics({f"auc/{key}": val}, step=trainer.global_step)
        print(f"\n  AUC  agnostic={aucs['agnostic']:.1%}  "
              f"wc={aucs['wc']:.1%}  wa={aucs['wa']:.1%}  wp={aucs['wp']:.1%}")


if __name__ == "__main__":
    main()
