#!/usr/bin/env python3
"""
Unified training and evaluation script for all six person detectors.

Supported detectors
-------------------
  algorithmic          — AlgorithmicDetector (rule-based; eval-only)
  drow                 — DrowDetector         (original DROW WNet3xLF2p)
  drspaam              — DrSpaamDetector      (DR-SPAAM: BeamNeighborConv + AttnSum)
  fullscan_cnn         — FullScanCNNDetector  (dilated CNN over beams + GRU)
  spacetime_cnn        — SpaceTimeCNNDetector (2-D conv over N_beams×T grid)
  fullscan_transformer — FullScanTransformerDetector (dilated CNN + beam attn + GRU)

Cutout-based detectors (drow, drspaam) consume:
  cutout(scans_hist, odoms_hist, N, nsamp=48)  →  (N_beams, T, 48)

Full-scan detectors (fullscan_cnn, spacetime_cnn, fullscan_transformer) consume:
  aligned_scan_xyz(scans_hist, odoms_hist, angles)  →  (T, N_beams, 3)
  transposed to (N_beams, T, 3)

Both types produce:
  logits : (N_beams, 4)   raw class logits  [bg, wc, wa, wp]
  votes  : (N_beams, 2)   vote offsets (dx, dy) in window space

Training losses
---------------
  Cross-entropy over 4 classes, class-weighted for background imbalance.
  MSE over vote offsets, restricted to positive (non-background) beams.

Usage
-----
  # Train DR-SPAAM on FROG for 10 epochs
  python train.py --detector drspaam --dataset frog --epochs 10

  # Train full-scan CNN on DROW, evaluate every 5 epochs
  python train.py --detector fullscan_cnn --dataset drow --auc-every 5

  # Evaluate a saved checkpoint
  python train.py --detector drspaam --dataset frog --weights out.pth --eval-only

  # Resume training
  python train.py --detector drspaam --resume out.pth --epochs 5
"""

import argparse
import random
from pathlib import Path
from types import SimpleNamespace

import numpy as np
import torch
import torch.nn.functional as F
from torch.optim import Adam, Optimizer
from tqdm import tqdm
from sklearn.metrics import auc as sklearn_auc


# ---------------------------------------------------------------------------
# Device detection (CUDA → DirectML → CPU)
# ---------------------------------------------------------------------------

def detect_device():
    """
    Detect the best available compute device.

    Priority: CUDA / ROCm  →  DirectML  →  CPU.

    DirectML (``torch-directml``) enables hardware-accelerated training on any
    DX12-capable GPU on Windows without requiring CUDA or ROCm drivers.
    Install it with: ``pip install torch-directml``

    Returns
    -------
    device    : str or torch_directml device
        Passed to ``model.to(device)`` and ``tensor.to(device)``.
    using_dml : bool
        True when DirectML is active (informational; the training loop
        handles DML the same way as CUDA via standard ``.to()`` calls).
    """
    if torch.cuda.is_available():
        name = torch.cuda.get_device_name(0)
        n    = torch.cuda.device_count()
        print(f"CUDA / ROCm: {name}  ({n} device(s))")
        return "cuda", False

    try:
        import torch_directml
        dml = torch_directml.device()
        print(f"DirectML: {dml}")
        return dml, True
    except ImportError:
        pass

    print("No GPU found — using CPU.")
    return "cpu", False


# ---------------------------------------------------------------------------
# DML-safe Adam (avoids lerp_, addcmul_, and other unsupported fused ops)
# ---------------------------------------------------------------------------

class _DmlAdam(Optimizer):
    """
    Adam optimizer implemented with only basic DML-compatible ops.

    PyTorch's built-in Adam uses ``lerp_`` and ``addcmul_`` for speed; both
    fall back to CPU on DirectML.  This class replaces them with equivalent
    ``mul_`` + ``add_`` sequences that stay on the DML device throughout.

    Mathematically identical to standard Adam (Kingma & Ba, 2015) with the
    same ``lr``, ``betas``, ``eps``, and ``weight_decay`` semantics.
    """

    def __init__(self, params, lr: float = 1e-3,
                 betas: tuple = (0.9, 0.999), eps: float = 1e-8,
                 weight_decay: float = 0.0):
        defaults = dict(lr=lr, betas=betas, eps=eps, weight_decay=weight_decay)
        super().__init__(params, defaults)

    @torch.no_grad()
    def step(self, closure=None):
        loss = None
        if closure is not None:
            with torch.enable_grad():
                loss = closure()

        for group in self.param_groups:
            lr           = group["lr"]
            beta1, beta2 = group["betas"]
            eps          = group["eps"]
            wd           = group["weight_decay"]

            for p in group["params"]:
                if p.grad is None:
                    continue

                grad  = p.grad
                state = self.state[p]

                if len(state) == 0:
                    state["step"]       = 0
                    state["exp_avg"]    = torch.zeros_like(p)
                    state["exp_avg_sq"] = torch.zeros_like(p)

                state["step"] += 1
                t            = state["step"]
                exp_avg      = state["exp_avg"]
                exp_avg_sq   = state["exp_avg_sq"]

                # Optional L2 weight decay
                if wd != 0.0:
                    grad = grad.add(p, alpha=wd)

                # m_t = beta1 * m_{t-1} + (1 - beta1) * g_t
                # (replaces exp_avg.lerp_(grad, 1 - beta1))
                exp_avg.mul_(beta1).add_(grad, alpha=1.0 - beta1)

                # v_t = beta2 * v_{t-1} + (1 - beta2) * g_t^2
                # (replaces exp_avg_sq.mul_(beta2).addcmul_(grad, grad, …))
                exp_avg_sq.mul_(beta2).add_(grad * grad, alpha=1.0 - beta2)

                # Bias-corrected step size
                bias_c1   = 1.0 - beta1 ** t
                bias_c2   = 1.0 - beta2 ** t
                step_size = lr * (bias_c2 ** 0.5) / bias_c1

                # p = p - step_size * m_t / (sqrt(v_t) + eps)
                denom = exp_avg_sq.sqrt().add_(eps)
                p.addcdiv_(exp_avg, denom, value=-step_size)

        return loss


def _make_optimizer(params, lr: float, weight_decay: float,
                    using_dml: bool) -> Optimizer:
    """Return the right Adam variant for the active device."""
    if using_dml:
        return _DmlAdam(params, lr=lr, weight_decay=weight_decay)
    return Adam(params, lr=lr, weight_decay=weight_decay, foreach=True)

from follow_the_drow.detectors import (
    AlgorithmicDetector, DrowDetector, DrSpaamDetector,
    FullScanCNNDetector, SpaceTimeCNNDetector, FullScanTransformerDetector,
    DETECTOR_REGISTRY,
)
from follow_the_drow.utils.drow_utils import (
    laser_angles, laser_minimum, laser_maximum, laser_increment,
    cutout, aligned_scan_xyz, rphi_to_xy, _win2global,
    votes_to_detections, _deep2flat, _process_detections,
)


# ---------------------------------------------------------------------------
# Dataset / configuration helpers
# ---------------------------------------------------------------------------

def _setup_datasets(args):
    """Load train and validation datasets; return (train_ds, val_ds, cfg)."""
    if args.dataset == "frog":
        from follow_the_drow.datasets import FROG_Dataset, frog_laser_angles
        print(f"Loading FROG train split ('{args.train_split}') …")
        train_ds = FROG_Dataset(split=args.train_split)
        val_ds = None
        if args.val_split:
            print(f"Loading FROG val split ('{args.val_split}') …")
            val_ds = FROG_Dataset(split=args.val_split)
        cfg = SimpleNamespace(
            name="frog",
            angles_fn=frog_laser_angles,
            fov_min=FROG_Dataset.LASER_MIN_ANGLE,
            fov_max=FROG_Dataset.LASER_MAX_ANGLE,
            laser_inc=FROG_Dataset.LASER_INCREMENT,
        )
    else:
        from follow_the_drow.datasets import DROW_Dataset
        from follow_the_drow.utils.file_utils import DROW_TRAIN_SET, DROW_VALIDATION_SET
        print("Loading DROW train set …")
        train_ds = DROW_Dataset(dataset=DROW_TRAIN_SET)
        val_ds = None
        if args.val_split:
            print("Loading DROW val set …")
            val_ds = DROW_Dataset(dataset=DROW_VALIDATION_SET)
        cfg = SimpleNamespace(
            name="drow",
            angles_fn=laser_angles,
            fov_min=laser_minimum,
            fov_max=laser_maximum,
            laser_inc=laser_increment,
        )

    n_train = sum(len(d) for d in train_ds.det_id)
    print(f"  Train: {len(train_ds.scan_id)} seq, {n_train} annotated frames")
    if val_ds is not None:
        n_val = sum(len(d) for d in val_ds.det_id)
        print(f"  Val:   {len(val_ds.scan_id)} seq, {n_val} annotated frames")
    print()
    return train_ds, val_ds, cfg


def _build_model(args):
    """Instantiate the requested detector."""
    det = args.detector
    if det == "drow":
        return DrowDetector(dropout=args.dropout,
                            time_frame_size=args.time_frame, verbose=False)
    if det == "drspaam":
        return DrSpaamDetector(n_time=args.time_frame, dropout=args.dropout)
    if det == "fullscan_cnn":
        return FullScanCNNDetector(n_time=args.time_frame)
    if det == "spacetime_cnn":
        return SpaceTimeCNNDetector(n_time=args.time_frame)
    if det == "fullscan_transformer":
        return FullScanTransformerDetector(n_time=args.time_frame)
    raise ValueError(f"'{det}' cannot be trained (algorithmic is eval-only)")


# ---------------------------------------------------------------------------
# Ground-truth target generation
# ---------------------------------------------------------------------------

def make_targets(scan: np.ndarray, angles: np.ndarray,
                 gt_per_class: dict, vote_collect_radius: float = 0.6):
    """
    Compute per-beam class labels and vote-offset targets.

    Parameters
    ----------
    scan          : (N,)  range measurements
    angles        : (N,)  beam angles in radians
    gt_per_class  : {class_id: [(r, phi), ...]}  1=wc, 2=wa, 3=wp
    vote_collect_radius : radius (m) around GT that marks a beam as positive

    Returns
    -------
    labels       : (N,) int64
    vote_targets : (N, 2) float32  — (dx, dy) in window space
    """
    scan   = np.asarray(scan,   dtype=np.float64)
    angles = np.asarray(angles, dtype=np.float64)
    N = len(scan)
    labels       = np.zeros(N, dtype=np.int64)
    vote_targets = np.zeros((N, 2), dtype=np.float32)

    beam_x = scan * -np.sin(angles)
    beam_y = scan *  np.cos(angles)

    for class_id, anns in sorted(gt_per_class.items()):
        for r_gt, phi_gt in anns:
            x_gt = r_gt * -np.sin(phi_gt)
            y_gt = r_gt *  np.cos(phi_gt)
            dist = np.sqrt((x_gt - beam_x) ** 2 + (y_gt - beam_y) ** 2)
            mask = dist < vote_collect_radius
            if not np.any(mask):
                continue
            labels[mask] = class_id
            dphi = phi_gt - angles[mask]
            vote_targets[mask, 0] = r_gt * np.sin(dphi)
            vote_targets[mask, 1] = r_gt * np.cos(dphi) - scan[mask]

    return labels, vote_targets


# ---------------------------------------------------------------------------
# Loss
# ---------------------------------------------------------------------------

def compute_loss(logits: torch.Tensor, votes: torch.Tensor,
                 labels: torch.Tensor, vote_targets: torch.Tensor,
                 vote_weight: float = 0.02):
    """
    logits       : (N, 4)  raw class logits
    votes        : (N, 2)  predicted vote offsets
    labels       : (N,)    int64 target class indices
    vote_targets : (N, 2)  target vote offsets
    """
    n_pos = int((labels > 0).sum())
    n_neg = int((labels == 0).sum())
    if n_pos > 0 and n_neg > 0:
        pos_weight = float(n_neg) / float(n_pos)
        weight = torch.ones(4, device=logits.device)
        weight[1:] = pos_weight
    else:
        weight = None

    l_class = F.cross_entropy(logits, labels, weight=weight)

    pos_mask = labels > 0
    if pos_mask.any():
        l_vote = F.mse_loss(votes[pos_mask], vote_targets[pos_mask])
    else:
        l_vote = logits.new_tensor(0.0)

    loss = l_class + vote_weight * l_vote
    return loss, l_class.item(), l_vote.item()


# ---------------------------------------------------------------------------
# Frame iterator
# ---------------------------------------------------------------------------

def iter_frames(dataset, cfg, subsample: float = 1.0, shuffle: bool = True):
    """
    Yield (seq, det_idx, scan, scans_hist, odoms_hist, gt_per_class) for
    every annotated frame.
    """
    indices = [
        (seq, det_idx)
        for seq in range(len(dataset.det_id))
        for det_idx in range(len(dataset.det_id[seq]))
    ]
    if shuffle:
        random.shuffle(indices)
    if subsample < 1.0:
        indices = indices[: max(1, int(len(indices) * subsample))]

    for seq, det_idx in indices:
        iscan = dataset.idet2iscan[seq][det_idx]
        scan  = dataset.scans[seq][iscan]
        scans_hist, odoms_hist = dataset.get_scan(seq, iscan, dataset.time_frame)
        gt_per_class = {
            1: dataset.det_wc[seq][det_idx],
            2: dataset.det_wa[seq][det_idx],
            3: dataset.det_wp[seq][det_idx],
        }
        yield seq, det_idx, scan, scans_hist, odoms_hist, gt_per_class


# ---------------------------------------------------------------------------
# Input extraction (detector-type aware)
# ---------------------------------------------------------------------------

def _extract_input(net, scan, scans_hist, odoms_hist, angles, cfg, device):
    """
    Build the model input tensor from raw scan data.

    Cutout-based detectors  → (N_beams, T, N_SAMP)
    Full-scan detectors     → (N_beams, T, 3)
    """
    if getattr(net, "INPUT_MODE", "cutout") == "full_scan":
        xyz = aligned_scan_xyz(scans_hist, odoms_hist, angles,
                               laser_inc=cfg.laser_inc)
        return torch.from_numpy(xyz.transpose(1, 0, 2)).to(device)  # (N_beams,T,3)
    else:
        nsamp = getattr(net, "N_SAMP", DrowDetector.N_SAMP)
        cut = cutout(scans_hist, odoms_hist, len(scan),
                     nsamp=nsamp, laserIncrement=cfg.laser_inc)
        return torch.from_numpy(cut).to(device)   # (N_beams, T, N_SAMP)


# ---------------------------------------------------------------------------
# Training
# ---------------------------------------------------------------------------

def train_epoch(net, dataset, cfg, optimizer,
                vote_radius: float, vote_weight: float,
                subsample: float = 1.0, device: str = "cpu",
                batch_size: int = 1):
    """
    One full training epoch.  Returns (avg_total, avg_class, avg_vote) losses.

    batch_size frames are accumulated before each optimizer step, so the
    effective mini-batch has shape (batch_size * N_beams, T, S).  Loss and
    class-weight statistics are computed jointly across all frames in the
    batch, which improves gradient quality when individual frames are sparse.
    """
    net.train()
    total_loss = class_loss = vote_loss = 0.0
    n_steps = 0

    n_frames = sum(len(d) for d in dataset.det_id)
    n_total  = max(1, int(n_frames * subsample)) if subsample < 1.0 else n_frames
    pbar = tqdm(total=n_total, desc="  train", unit="fr",
                leave=False, dynamic_ncols=True)

    buf_x, buf_labels, buf_votes = [], [], []

    def _optimizer_step():
        nonlocal total_loss, class_loss, vote_loss, n_steps
        x_bat = torch.cat(buf_x,      dim=0)
        l_bat = torch.cat(buf_labels, dim=0)
        v_bat = torch.cat(buf_votes,  dim=0)
        logits, vpred = net(x_bat)
        loss, lc, lv  = compute_loss(logits, vpred, l_bat, v_bat, vote_weight)
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        total_loss += loss.item()
        class_loss += lc
        vote_loss  += lv
        n_steps    += 1
        pbar.set_postfix(loss=f"{total_loss / n_steps:.4f}", refresh=False)
        buf_x.clear(); buf_labels.clear(); buf_votes.clear()

    for _, _, scan, scans_hist, odoms_hist, gt_per_class in iter_frames(
            dataset, cfg, subsample=subsample, shuffle=True):
        angles = cfg.angles_fn(len(scan))
        labels, vote_targets = make_targets(scan, angles, gt_per_class,
                                            vote_collect_radius=vote_radius)
        buf_x.append(_extract_input(net, scan, scans_hist, odoms_hist,
                                    angles, cfg, device))
        buf_labels.append(torch.from_numpy(labels).long().to(device))
        buf_votes.append(torch.from_numpy(vote_targets).to(device))
        pbar.update(1)
        if len(buf_x) >= batch_size:
            _optimizer_step()

    if buf_x:           # flush the last partial batch
        _optimizer_step()

    pbar.close()
    n = max(n_steps, 1)
    return total_loss / n, class_loss / n, vote_loss / n


# ---------------------------------------------------------------------------
# Evaluation
# ---------------------------------------------------------------------------

def evaluate_loss(net, dataset, cfg,
                  vote_radius: float, vote_weight: float,
                  subsample: float = 1.0, device: str = "cpu",
                  batch_size: int = 1):
    """Compute average loss on the given dataset (no gradient)."""
    net.eval()
    total_loss = class_loss = vote_loss = 0.0
    n_steps = 0

    n_frames = sum(len(d) for d in dataset.det_id)
    n_total  = max(1, int(n_frames * subsample)) if subsample < 1.0 else n_frames
    pbar = tqdm(total=n_total, desc="    val", unit="fr",
                leave=False, dynamic_ncols=True)

    buf_x, buf_labels, buf_votes = [], [], []

    def _eval_batch():
        nonlocal total_loss, class_loss, vote_loss, n_steps
        x_bat = torch.cat(buf_x,      dim=0)
        l_bat = torch.cat(buf_labels, dim=0)
        v_bat = torch.cat(buf_votes,  dim=0)
        logits, vpred = net(x_bat)
        loss, lc, lv  = compute_loss(logits, vpred, l_bat, v_bat, vote_weight)
        total_loss += loss.item()
        class_loss += lc
        vote_loss  += lv
        n_steps    += 1
        pbar.set_postfix(loss=f"{total_loss / n_steps:.4f}", refresh=False)
        buf_x.clear(); buf_labels.clear(); buf_votes.clear()

    with torch.no_grad():
        for _, _, scan, scans_hist, odoms_hist, gt_per_class in iter_frames(
                dataset, cfg, subsample=subsample, shuffle=False):
            angles = cfg.angles_fn(len(scan))
            labels, vote_targets = make_targets(scan, angles, gt_per_class,
                                                vote_collect_radius=vote_radius)
            buf_x.append(_extract_input(net, scan, scans_hist, odoms_hist,
                                        angles, cfg, device))
            buf_labels.append(torch.from_numpy(labels).long().to(device))
            buf_votes.append(torch.from_numpy(vote_targets).to(device))
            pbar.update(1)
            if len(buf_x) >= batch_size:
                _eval_batch()

        if buf_x:
            _eval_batch()

    pbar.close()
    n = max(n_steps, 1)
    return total_loss / n, class_loss / n, vote_loss / n


def _safe_auc(recs, precs):
    mask = ~np.isnan(recs) & ~np.isnan(precs)
    r, p = recs[mask], precs[mask]
    if len(r) < 2:
        return float("nan")
    order = np.argsort(r)
    return float(sklearn_auc(r[order], p[order]))


def evaluate_auc(net, dataset, cfg, eval_r: float = 0.5,
                 v2d_conf: dict = None, device: str = "cpu",
                 batch_size: int = 1):
    """
    Run full inference on every annotated frame and compute per-class AUC.

    Works with any detector that has INPUT_MODE set ("cutout" or "full_scan").

    Returns a dict with keys 'agnostic', 'wc', 'wa', 'wp'.
    """
    if v2d_conf is None:
        v2d_conf = {
            "blur_sigma":          2.0,
            "blur_win":            11,
            "bin_size":            0.1,
            "vote_collect_radius": 0.5,
            "min_thresh":          1e-3,
        }

    net.eval()
    all_confs, all_votes_raw = [], []
    all_scans_for_pr = []
    all_wcs, all_was, all_wps = [], [], []

    n_frames = sum(len(dataset.det_id[seq]) for seq in range(len(dataset.det_id)))

    # Collect all annotated (seq, det_idx) pairs in order
    all_pairs = [
        (seq, det_idx)
        for seq in range(len(dataset.det_id))
        for det_idx in range(len(dataset.det_id[seq]))
    ]

    with torch.no_grad():
        pbar = tqdm(total=n_frames, desc="    AUC", unit="fr",
                    leave=False, dynamic_ncols=True)

        buf_x, buf_meta = [], []

        def _flush_auc():
            x_bat = torch.cat(buf_x, dim=0)         # (B*N_beams, T, S)
            logits, vpred = net(x_bat)
            confs_np = F.softmax(logits, dim=-1).cpu().numpy()
            votes_np = vpred.cpu().numpy()
            n_beams  = buf_x[0].shape[0]            # beams per frame
            for i, (scan_i, wc_i, wa_i, wp_i) in enumerate(buf_meta):
                s, e = i * n_beams, (i + 1) * n_beams
                all_confs.append(confs_np[s:e])
                all_votes_raw.append(votes_np[s:e])
                all_scans_for_pr.append(scan_i)
                all_wcs.append(wc_i)
                all_was.append(wa_i)
                all_wps.append(wp_i)
            buf_x.clear(); buf_meta.clear()

        for seq, det_idx in all_pairs:
            iscan = dataset.idet2iscan[seq][det_idx]
            scan  = dataset.scans[seq][iscan]
            scans_hist, odoms_hist = dataset.get_scan(seq, iscan, dataset.time_frame)
            angles = cfg.angles_fn(len(scan))

            buf_x.append(_extract_input(net, scan, scans_hist, odoms_hist,
                                        angles, cfg, device))
            buf_meta.append((scan,
                             dataset.det_wc[seq][det_idx],
                             dataset.det_wa[seq][det_idx],
                             dataset.det_wp[seq][det_idx]))
            pbar.update(1)
            if len(buf_x) >= batch_size:
                _flush_auc()

        if buf_x:
            _flush_auc()

        pbar.close()

    angles = cfg.angles_fn(len(all_scans_for_pr[0]))
    all_x_votes, all_y_votes = [], []
    for scan, vots in zip(all_scans_for_pr, all_votes_raw):
        r_new, phi_new = _win2global(
            scan[None], angles[None],
            vots[None, :, 0], vots[None, :, 1],
        )
        all_x_votes.append(r_new * -np.sin(phi_new))
        all_y_votes.append(r_new *  np.cos(phi_new))

    x_arr = np.concatenate(all_x_votes, axis=0)
    y_arr = np.concatenate(all_y_votes, axis=0)
    p_arr = np.stack(all_confs, axis=0)

    detections = votes_to_detections(x_arr, y_arr, p_arr, **v2d_conf)
    det_x, det_y, det_p, det_f = _deep2flat(detections)

    wd, wc, wa, wp = _process_detections(
        det_x, det_y, det_p, det_f,
        all_wcs, all_was, all_wps, eval_r,
    )
    return {
        "agnostic": _safe_auc(*wd[:2]),
        "wc":       _safe_auc(*wc[:2]),
        "wa":       _safe_auc(*wa[:2]),
        "wp":       _safe_auc(*wp[:2]),
    }


# ---------------------------------------------------------------------------
# Checkpoint helpers
# ---------------------------------------------------------------------------

def save_checkpoint(path: Path, net, optimizer, epoch: int,
                    detector_name: str, dataset_name: str):
    torch.save({
        "detector":  detector_name,
        "model":     net.state_dict(),
        "optimizer": optimizer.state_dict(),
        "epoch":     epoch,
        "dataset":   dataset_name,
    }, path)
    print(f"  Checkpoint saved → {path}")


def load_checkpoint(path: Path, net, optimizer=None):
    ckpt = torch.load(path, map_location="cpu")
    net.load_state_dict(ckpt["model"])
    if optimizer is not None and "optimizer" in ckpt:
        optimizer.load_state_dict(ckpt["optimizer"])
    epoch = ckpt.get("epoch", 0)
    print(f"  Loaded checkpoint from {path}  (epoch {epoch})")
    return epoch


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Train and evaluate person detectors on DROW or FROG.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    # Detector selection
    parser.add_argument(
        "--detector",
        choices=list(DETECTOR_REGISTRY),
        default="drspaam",
        help="Detector to train/evaluate (default: drspaam)",
    )
    # Dataset
    parser.add_argument("--dataset",     choices=["drow", "frog"], default="frog")
    parser.add_argument("--train-split", default="train",
                        help="Training split (default: train)")
    parser.add_argument("--val-split",   default="val",
                        help="Validation split (default: val; '' to disable)")
    # Training
    parser.add_argument("--epochs",      type=int,   default=10)
    parser.add_argument("--lr",          type=float, default=1e-3)
    parser.add_argument("--weight-decay",type=float, default=1e-4)
    parser.add_argument("--dropout",     type=float, default=0.5,
                        help="Dropout (used by drow/drspaam; default: 0.5)")
    parser.add_argument("--time-frame",  type=int,   default=5,
                        help="Number of scans in the temporal window (default: 5)")
    parser.add_argument("--vote-radius", type=float, default=0.6)
    parser.add_argument("--vote-weight", type=float, default=0.02)
    parser.add_argument("--subsample",   type=float, default=1.0,
                        help="Fraction of frames per epoch (use 0.05 for quick dev)")
    parser.add_argument("--batch-size",  type=int,   default=4,
                        help="Frames per optimizer step; effective beam batch = "
                             "batch_size × N_beams (default: 4)")
    # Checkpointing
    parser.add_argument("--out",    type=Path, default=Path("weights_trained.pth"))
    parser.add_argument("--resume", type=Path, default=None,
                        help="Resume from checkpoint")
    parser.add_argument("--weights",type=Path, default=None,
                        help="Load weights (eval-only mode)")
    # Evaluation
    parser.add_argument("--eval-only",  action="store_true")
    parser.add_argument("--eval-r",     type=float, default=0.5)
    parser.add_argument("--auc-every",  type=int,   default=0,
                        help="Compute full AUC every N epochs (0 = end only)")
    args = parser.parse_args()
    args.val_split = args.val_split.strip() or None

    # ------------------------------------------------------------------
    # AlgorithmicDetector: eval-only (not an nn.Module)
    # ------------------------------------------------------------------
    if args.detector == "algorithmic":
        print("AlgorithmicDetector is not trainable — running eval only.")
        dataset, cfg = _setup_datasets(SimpleNamespace(
            dataset=args.dataset,
            train_split=args.train_split,
            val_split=None,
        ))[:2]
        # Algorithmic detector does not produce logits/votes; skip AUC pipeline.
        print("(Algorithmic AUC computation via the standard pipeline "
              "is not supported — use compare_detectors.py instead.)")
        return

    # ------------------------------------------------------------------
    # Setup
    # ------------------------------------------------------------------
    device, using_dml = detect_device()
    if using_dml:
        print("  Note: DirectML — tensors moved to DML device via .to().\n")
    else:
        print(f"  Device: {device}\n")

    train_ds, val_ds, cfg = _setup_datasets(args)

    net = _build_model(args).to(device)
    optimizer = _make_optimizer(net.parameters(), lr=args.lr,
                                weight_decay=args.weight_decay,
                                using_dml=using_dml)

    start_epoch = 0
    if args.resume:
        start_epoch = load_checkpoint(args.resume, net, optimizer)
    elif args.weights:
        load_checkpoint(args.weights, net)

    n_params = sum(p.numel() for p in net.parameters())
    print(f"Model: {args.detector}  —  {n_params:,} parameters\n")

    # ------------------------------------------------------------------
    # Eval-only mode
    # ------------------------------------------------------------------
    if args.eval_only:
        eval_ds = val_ds or train_ds
        print("=== Evaluation ===")
        aucs = evaluate_auc(net, eval_ds, cfg, eval_r=args.eval_r, device=device,
                            batch_size=args.batch_size)
        print(f"  Agnostic (any) : {aucs['agnostic']:.1%}")
        print(f"  Wheelchair(wc) : {aucs['wc']:.1%}")
        print(f"  Walker    (wa) : {aucs['wa']:.1%}")
        print(f"  Person    (wp) : {aucs['wp']:.1%}")
        return

    # ------------------------------------------------------------------
    # Training loop
    # ------------------------------------------------------------------
    print(f"Training {args.detector} for {args.epochs} epoch(s), "
          f"subsample={args.subsample:.0%} …\n")

    epoch_bar = tqdm(range(start_epoch + 1, start_epoch + args.epochs + 1),
                     desc="Epochs", unit="ep", dynamic_ncols=True)

    for epoch in epoch_bar:
        tl, tc, tv = train_epoch(
            net, train_ds, cfg, optimizer,
            vote_radius=args.vote_radius, vote_weight=args.vote_weight,
            subsample=args.subsample, device=device, batch_size=args.batch_size,
        )
        msg = (f"Epoch {epoch:3d}/{start_epoch + args.epochs}  "
               f"train_loss={tl:.4f}  (class={tc:.4f}, vote={tv:.4f})")

        if val_ds is not None:
            vl, vc, vv = evaluate_loss(
                net, val_ds, cfg,
                vote_radius=args.vote_radius, vote_weight=args.vote_weight,
                subsample=min(args.subsample * 2, 1.0), device=device,
                batch_size=args.batch_size,
            )
            msg += f"  |  val_loss={vl:.4f}  (class={vc:.4f}, vote={vv:.4f})"

        tqdm.write(msg)

        if args.auc_every > 0 and epoch % args.auc_every == 0:
            auc_ds = val_ds or train_ds
            aucs = evaluate_auc(net, auc_ds, cfg, eval_r=args.eval_r, device=device,
                                batch_size=args.batch_size)
            tqdm.write(f"  AUC  agnostic={aucs['agnostic']:.1%}  "
                       f"wc={aucs['wc']:.1%}  wa={aucs['wa']:.1%}  wp={aucs['wp']:.1%}")

    # ------------------------------------------------------------------
    # Final AUC + save
    # ------------------------------------------------------------------
    print()
    if val_ds is not None:
        print("Computing final AUC on val set …")
        aucs = evaluate_auc(net, val_ds, cfg, eval_r=args.eval_r, device=device,
                            batch_size=args.batch_size)
        print(f"  Agnostic (any) : {aucs['agnostic']:.1%}")
        print(f"  Wheelchair(wc) : {aucs['wc']:.1%}")
        print(f"  Walker    (wa) : {aucs['wa']:.1%}")
        print(f"  Person    (wp) : {aucs['wp']:.1%}")
        print()

    save_checkpoint(args.out, net, optimizer,
                    epoch=start_epoch + args.epochs,
                    detector_name=args.detector,
                    dataset_name=cfg.name)


if __name__ == "__main__":
    main()


# ---------------------------------------------------------------------------
# Backward-compatibility alias (train_lightning.py imports this name)
# ---------------------------------------------------------------------------
_setup_train = _setup_datasets
