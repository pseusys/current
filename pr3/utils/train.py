#!/usr/bin/env python3
"""
Training and evaluation script for the three custom full-scan person detectors.

DROW and DR-SPAAM use published pre-trained weights and are not trained here;
use evaluate.py --drow / --drspaam to evaluate them.

Trainable detectors
-------------------
  fullscan_cnn         — FullScanCNNDetector  (dilated CNN over beams + GRU)
  spacetime_cnn        — SpaceTimeCNNDetector (2-D conv over N_beams×T grid)
  fullscan_transformer — FullScanTransformerDetector (dilated CNN + beam attn + GRU)

Eval-only
---------
  algorithmic          — AlgorithmicDetector (rule-based; see evaluate.py)

All detectors consume full-scan input:
  aligned_scan_xyz(scans_hist, odoms_hist, angles)  →  (T, N_beams, 3)
  transposed to (N_beams, T, 3)

All detectors produce:
  logits : (N_beams, 4)   raw class logits  [bg, wc, wa, wp]
  votes  : (N_beams, 2)   vote offsets (dx, dy) in window space

Training losses
---------------
  Cross-entropy over 4 classes, class-weighted for background imbalance.
  MSE over vote offsets, restricted to positive (non-background) beams.

Usage
-----
  # Train full-scan CNN on FROG for 10 epochs
  python train.py --detector fullscan_cnn --dataset frog --epochs 10

  # Train on DROW, evaluate AUC every 5 epochs
  python train.py --detector fullscan_cnn --dataset drow --auc-every 5

  # Evaluate a saved checkpoint
  python train.py --detector fullscan_cnn --weights out.pth --eval-only

  # Resume training
  python train.py --detector spacetime_cnn --resume out.pth --epochs 5

  # Train with early stopping and cosine LR schedule
  python train.py --detector fullscan_cnn --epochs 50 --patience 10 --lr-schedule cosine

  # Tune FullScanTransformer architecture
  python train.py --detector fullscan_transformer --backbone-channels 128 --hidden 256 --n-heads 4
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
from torch.utils.data import Dataset, DataLoader, Subset


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
    cutout, aligned_scan_xyz, project_cartesian_from_polar, _win2global,
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
    """
    Instantiate the requested detector with all hyperparameters.

    Supports architecture tuning for the three full-scan models:
      --dropout           applied to all models (default 0.5)
      --backbone-channels DilatedScanBackbone output channels (FullScanCNN/Transformer)
      --hidden            GRU hidden size (FullScanCNN/Transformer)
      --n-heads           BeamSelfAttention heads (FullScanTransformer)
      --out-channels      final channels (SpaceTimeCNN)
    """
    det = args.detector
    dr  = getattr(args, "dropout",           0.5)
    bc  = getattr(args, "backbone_channels", 64)
    hid = getattr(args, "hidden",            128)
    nh  = getattr(args, "n_heads",           8)
    oc  = getattr(args, "out_channels",      128)
    tf  = getattr(args, "time_frame",        5)

    if det == "drow":
        return DrowDetector(dropout=dr, time_frame_size=tf, verbose=False)
    if det == "drspaam":
        return DrSpaamDetector(dropout=dr, num_scans=tf)
    if det == "fullscan_cnn":
        return FullScanCNNDetector(n_time=tf, backbone_channels=bc,
                                   hidden=hid, dropout=dr)
    if det == "spacetime_cnn":
        return SpaceTimeCNNDetector(n_time=tf, out_channels=oc, dropout=dr)
    if det == "fullscan_transformer":
        return FullScanTransformerDetector(n_time=tf, backbone_channels=bc,
                                           n_heads=nh, hidden=hid, dropout=dr)
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
# Cached frame dataset  (speeds up multi-epoch training by ~2–4×)
# ---------------------------------------------------------------------------

class LidarFrameDataset(Dataset):
    """
    One item = one annotated lidar frame, pre-processed and cached in RAM.

    On first access per index the frame is loaded from the raw dataset,
    preprocessed (cutout or aligned_scan_xyz), and stored in ``_cache``.
    Subsequent accesses (later epochs) are pure dict lookups — no NumPy
    preprocessing.  Memory per frame: ~430 KB (DROW) / ~675 KB (FROG).

    Parameters
    ----------
    dataset     : DROW_Dataset or FROG_Dataset
    cfg         : SimpleNamespace with angles_fn and laser_inc
    input_mode  : "cutout" or "full_scan"
    vote_radius : positive-beam radius around GT (same as vote_collect_radius)
    nsamp       : number of cutout samples per beam (ignored for full_scan)
    """

    def __init__(self, dataset, cfg, input_mode: str,
                 vote_radius: float, nsamp: int = 48):
        self._dataset     = dataset
        self._cfg         = cfg
        self._input_mode  = input_mode
        self._vote_radius = vote_radius
        self._nsamp       = nsamp
        self._angles      = cfg.angles_fn(dataset.scans[0].shape[1])
        self.indices      = [
            (seq, det_idx)
            for seq in range(len(dataset.det_id))
            for det_idx in range(len(dataset.det_id[seq]))
        ]
        self._cache: dict = {}

    def __len__(self) -> int:
        return len(self.indices)

    def __getitem__(self, idx: int):
        if idx in self._cache:
            return self._cache[idx]

        seq, det_idx = self.indices[idx]
        ds    = self._dataset
        iscan = ds.idet2iscan[seq][det_idx]
        scan  = ds.scans[seq][iscan]
        scans_hist, odoms_hist = ds.get_scan(seq, iscan, ds.time_frame)

        labels, vote_targets = make_targets(
            scan, self._angles,
            {1: ds.det_wc[seq][det_idx],
             2: ds.det_wa[seq][det_idx],
             3: ds.det_wp[seq][det_idx]},
            self._vote_radius,
        )

        if self._input_mode == "full_scan":
            arr = aligned_scan_xyz(scans_hist, odoms_hist, self._angles,
                                   laser_inc=self._cfg.laser_inc)
            x = torch.from_numpy(
                np.asarray(arr, dtype=np.float32).transpose(1, 0, 2))  # (N,T,3)
        else:
            arr = cutout(scans_hist, odoms_hist, len(scan),
                         nsamp=self._nsamp,
                         laserIncrement=self._cfg.laser_inc)
            x = torch.from_numpy(np.asarray(arr, dtype=np.float32))    # (N,T,S)

        result = (
            x,
            torch.from_numpy(labels),
            torch.from_numpy(vote_targets),
        )
        self._cache[idx] = result
        return result


# ---------------------------------------------------------------------------
# Training
# ---------------------------------------------------------------------------

def train_epoch(net, frame_ds: LidarFrameDataset, optimizer,
                vote_weight: float, subsample: float = 1.0,
                device: str = "cpu", batch_size: int = 4):
    """One training epoch using the cached LidarFrameDataset."""
    if subsample < 1.0:
        n = max(1, int(len(frame_ds) * subsample))
        idx = random.sample(range(len(frame_ds)), n)
        ds = Subset(frame_ds, idx)
    else:
        ds = frame_ds

    loader = DataLoader(ds, batch_size=batch_size, shuffle=True, num_workers=0)

    net.train()
    total_loss = class_loss = vote_loss = 0.0
    n_steps = 0
    pbar = tqdm(loader, desc="  train", unit="fr", leave=False, dynamic_ncols=True)

    _beam_batch = getattr(net, "BEAM_BATCH", False)
    for x, labels, votes in pbar:
        B, N = x.shape[:2]
        # BEAM_BATCH models (DrSpaamDetector) need (B, N, T, S) so that spatial
        # attention sees all N beams of one scan together.  Other models use the
        # flattened (B*N, T, S) as before.
        x_in   = x.to(device) if _beam_batch else x.reshape(B*N, *x.shape[2:]).to(device)
        l_flat  = labels.reshape(B * N).to(device)
        v_flat  = votes.reshape(B * N, 2).to(device)
        logits, vpred = net(x_in)          # always returns (B*N, 4/2)
        loss, lc, lv  = compute_loss(logits, vpred, l_flat, v_flat, vote_weight)
        optimizer.zero_grad(set_to_none=True)
        loss.backward()
        optimizer.step()
        total_loss += loss.item()
        class_loss += lc
        vote_loss  += lv
        n_steps    += 1
        pbar.set_postfix(loss=f"{total_loss / n_steps:.4f}", refresh=False)

    n = max(n_steps, 1)
    return total_loss / n, class_loss / n, vote_loss / n


# ---------------------------------------------------------------------------
# Evaluation
# ---------------------------------------------------------------------------

def evaluate_loss(net, frame_ds: LidarFrameDataset, vote_weight: float,
                  subsample: float = 1.0, device: str = "cpu",
                  batch_size: int = 4):
    """Compute average loss on a cached LidarFrameDataset (no gradient)."""
    if subsample < 1.0:
        n = max(1, int(len(frame_ds) * min(subsample * 2, 1.0)))
        idx = random.sample(range(len(frame_ds)), n)
        ds = Subset(frame_ds, idx)
    else:
        ds = frame_ds

    loader = DataLoader(ds, batch_size=batch_size, shuffle=False, num_workers=0)

    net.eval()
    total_loss = class_loss = vote_loss = 0.0
    n_steps = 0
    pbar = tqdm(loader, desc="    val", unit="fr", leave=False, dynamic_ncols=True)

    _beam_batch = getattr(net, "BEAM_BATCH", False)
    with torch.no_grad():
        for x, labels, votes in pbar:
            B, N = x.shape[:2]
            x_in   = x.to(device) if _beam_batch else x.reshape(B*N, *x.shape[2:]).to(device)
            l_flat  = labels.reshape(B * N).to(device)
            v_flat  = votes.reshape(B * N, 2).to(device)
            logits, vpred = net(x_in)
            loss, lc, lv  = compute_loss(logits, vpred, l_flat, v_flat, vote_weight)
            total_loss += loss.item()
            class_loss += lc
            vote_loss  += lv
            n_steps    += 1
            pbar.set_postfix(loss=f"{total_loss / n_steps:.4f}", refresh=False)

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

    _angles = cfg.angles_fn(dataset.scans[0].shape[1])

    with torch.no_grad():
        pbar = tqdm(total=n_frames, desc="    AUC", unit="fr",
                    leave=False, dynamic_ncols=True)

        buf_x, buf_meta = [], []

        _beam_batch = getattr(net, "BEAM_BATCH", False)

        def _flush_auc():
            # BEAM_BATCH models need all N beams of each scan together: stack not cat.
            if _beam_batch:
                x_bat = torch.stack(buf_x, dim=0)   # (B, N_beams, T, S)
            else:
                x_bat = torch.cat(buf_x, dim=0)     # (B*N_beams, T, S)
            logits, vpred = net(x_bat)              # always (B*N_beams, n_cls/2)
            if logits.shape[-1] == 1:
                # pedestrian_only published weights: sigmoid + expand to 4-class
                prob  = torch.sigmoid(logits)
                confs = torch.zeros(logits.shape[0], 4, device=logits.device)
                confs[:, 3] = prob[:, 0]
                confs_np = confs.cpu().numpy()
            else:
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
            angles = _angles

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

    angles = _angles
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
    print(f"  Checkpoint saved -> {path}")


def load_checkpoint(path: Path, net, optimizer=None):
    ckpt = torch.load(path, map_location="cpu")
    net.load_state_dict(ckpt["model"])
    if optimizer is not None and "optimizer" in ckpt:
        optimizer.load_state_dict(ckpt["optimizer"])
    epoch = ckpt.get("epoch", 0)
    print(f"  Loaded checkpoint from {path}  (epoch {epoch})")
    return epoch


# ---------------------------------------------------------------------------
# Defaults helper (for programmatic / notebook use)
# ---------------------------------------------------------------------------

def _default_args(**overrides) -> SimpleNamespace:
    """
    Return a SimpleNamespace with all training defaults.

    Useful for notebook cells or scripts that call train_model() directly
    without going through argparse.  Keyword arguments override defaults.

    Example
    -------
    >>> args = _default_args(detector="drspaam", epochs=20, patience=10)
    >>> history = train_model(args)

    Notes
    -----
    GRU-based models (fullscan_cnn, fullscan_transformer) are not compatible
    with DirectML.  Pass force_cpu=True for those models when DirectML is
    active.
    """
    defaults = dict(
        detector="fullscan_cnn",
        dataset="frog",
        train_split="train",
        val_split="val",
        epochs=20,
        lr=1e-3,
        weight_decay=1e-4,
        dropout=0.5,
        time_frame=5,
        vote_radius=0.6,
        vote_weight=0.02,
        subsample=1.0,
        batch_size=4,
        # full-scan architecture hyperparams
        backbone_channels=64,
        hidden=128,
        n_heads=8,
        out_channels=128,
        # regularisation / scheduling
        patience=5,
        lr_schedule="plateau",
        # checkpoint
        out=Path("weights_trained.pth"),
        resume=None,
        weights=None,
        # evaluation
        eval_only=False,
        eval_r=0.5,
        auc_every=5,
        # device override: set True for GRU models when DirectML is active
        force_cpu=False,
    )
    defaults.update(overrides)
    return SimpleNamespace(**defaults)


# ---------------------------------------------------------------------------
# Main training routine
# ---------------------------------------------------------------------------

def train_model(args) -> dict:
    """
    Run the full training pipeline for args.epochs epochs.

    Parameters
    ----------
    args : argparse.Namespace or SimpleNamespace
        All training options.  Call _default_args(**overrides) to get a
        fully-populated namespace for programmatic use.

    Returns
    -------
    dict with keys:
        epochs            list[int]    epoch numbers recorded
        train_loss        list[float]
        train_class_loss  list[float]
        train_vote_loss   list[float]
        val_loss          list[float]  (empty when no val split)
        val_class_loss    list[float]
        val_vote_loss     list[float]
        auc_epochs        list[int]    epochs at which AUC was evaluated
        val_auc_agnostic  list[float]
        val_auc_wc        list[float]
        val_auc_wa        list[float]
        val_auc_wp        list[float]
        stopped_epoch     int          last epoch trained

    Notes
    -----
    Early stopping
        Requires a val split (--val-split / args.val_split).  When patience>0
        and val_loss does not improve for <patience> epochs, training stops and
        the best checkpoint is loaded before saving the final weights.
        Best checkpoint is written to <out>.best.pth.

    LR scheduling
        "cosine"  — CosineAnnealingLR over args.epochs, eta_min = lr/100
        "plateau" — ReduceLROnPlateau on val_loss (requires val split)
        "none"    — constant learning rate
    """
    # Device
    force_cpu = getattr(args, "force_cpu", False)
    if force_cpu:
        device, using_dml = "cpu", False
        print("  [INFO] force_cpu=True — using CPU regardless of GPU availability.\n")
    else:
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

    # Build cached frame datasets once — preprocessing is amortised across epochs
    _input_mode = getattr(net, "INPUT_MODE", "cutout")
    _nsamp      = getattr(net, "N_SAMP", DrowDetector.N_SAMP)
    train_frame_ds = LidarFrameDataset(
        train_ds, cfg, _input_mode, args.vote_radius, _nsamp)
    val_frame_ds = (LidarFrameDataset(
        val_ds, cfg, _input_mode, args.vote_radius, _nsamp)
        if val_ds is not None else None)

    n_params = sum(p.numel() for p in net.parameters())
    print(f"Model: {args.detector}  —  {n_params:,} parameters\n")

    # LR scheduler
    schedule = getattr(args, "lr_schedule", "none")
    if schedule == "cosine":
        scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(
            optimizer, T_max=args.epochs, eta_min=args.lr * 1e-2)
    elif schedule == "plateau":
        if val_ds is None:
            print("  [WARN] lr_schedule=plateau requires a val split; "
                  "scheduler disabled.\n")
            scheduler = None
        else:
            scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
                optimizer, patience=max(3, args.epochs // 10),
                factor=0.5, verbose=True)
    else:
        scheduler = None

    # Early stopping
    patience = getattr(args, "patience", 0)
    if patience > 0 and val_ds is None:
        print("  [WARN] patience>0 requires a val split; early stopping disabled.\n")
        patience = 0

    best_val_loss    = float("inf")
    patience_counter = 0
    best_path = args.out.with_suffix(".best.pth") if patience > 0 else None

    history = dict(
        epochs=[],
        train_loss=[], train_class_loss=[], train_vote_loss=[],
        val_loss=[], val_class_loss=[], val_vote_loss=[],
        auc_epochs=[],
        val_auc_agnostic=[], val_auc_wc=[], val_auc_wa=[], val_auc_wp=[],
        stopped_epoch=start_epoch,
    )

    print(f"Training {args.detector} for {args.epochs} epoch(s), "
          f"subsample={args.subsample:.0%} …\n")

    epoch_bar = tqdm(range(start_epoch + 1, start_epoch + args.epochs + 1),
                     desc="Epochs", unit="ep", dynamic_ncols=True)

    final_epoch = start_epoch
    for epoch in epoch_bar:
        tl, tc, tv = train_epoch(
            net, train_frame_ds, optimizer,
            vote_weight=args.vote_weight,
            subsample=args.subsample, device=device, batch_size=args.batch_size,
        )
        history["epochs"].append(epoch)
        history["train_loss"].append(tl)
        history["train_class_loss"].append(tc)
        history["train_vote_loss"].append(tv)

        msg = (f"Epoch {epoch:3d}/{start_epoch + args.epochs}  "
               f"train_loss={tl:.4f}  (class={tc:.4f}, vote={tv:.4f})")

        stop = False
        if val_ds is not None:
            vl, vc, vv = evaluate_loss(
                net, val_frame_ds,
                vote_weight=args.vote_weight,
                subsample=args.subsample, device=device,
                batch_size=args.batch_size,
            )
            history["val_loss"].append(vl)
            history["val_class_loss"].append(vc)
            history["val_vote_loss"].append(vv)
            msg += f"  |  val_loss={vl:.4f}  (class={vc:.4f}, vote={vv:.4f})"

            if isinstance(scheduler, torch.optim.lr_scheduler.ReduceLROnPlateau):
                scheduler.step(vl)

            # Early stopping
            if patience > 0:
                if vl < best_val_loss:
                    best_val_loss = vl
                    patience_counter = 0
                    save_checkpoint(best_path, net, optimizer, epoch,
                                    args.detector, cfg.name)
                else:
                    patience_counter += 1
                    msg += f"  patience={patience_counter}/{patience}"
                    if patience_counter >= patience:
                        tqdm.write(msg)
                        tqdm.write(
                            f"  Early stopping at epoch {epoch} "
                            f"(no val improvement for {patience} epochs)."
                        )
                        final_epoch = epoch
                        stop = True

        if scheduler is not None and not isinstance(
                scheduler, torch.optim.lr_scheduler.ReduceLROnPlateau):
            scheduler.step()

        tqdm.write(msg)

        # AUC evaluation
        auc_every = getattr(args, "auc_every", 0)
        if auc_every > 0 and epoch % auc_every == 0:
            auc_ds = val_ds or train_ds
            aucs = evaluate_auc(net, auc_ds, cfg, eval_r=args.eval_r,
                                device=device, batch_size=args.batch_size)
            history["auc_epochs"].append(epoch)
            history["val_auc_agnostic"].append(aucs["agnostic"])
            history["val_auc_wc"].append(aucs["wc"])
            history["val_auc_wa"].append(aucs["wa"])
            history["val_auc_wp"].append(aucs["wp"])
            tqdm.write(f"  AUC  agnostic={aucs['agnostic']:.1%}  "
                       f"wc={aucs['wc']:.1%}  wa={aucs['wa']:.1%}  "
                       f"wp={aucs['wp']:.1%}")

        final_epoch = epoch
        if stop:
            epoch_bar.close()
            break

    history["stopped_epoch"] = final_epoch

    # Final AUC (skip if already computed for this epoch)
    print()
    if val_ds is not None:
        last_auc_epoch = history["auc_epochs"][-1] if history["auc_epochs"] else -1
        if last_auc_epoch != final_epoch:
            print("Computing final AUC on val set …")
            aucs = evaluate_auc(net, val_ds, cfg, eval_r=args.eval_r,
                                device=device, batch_size=args.batch_size)
            history["auc_epochs"].append(final_epoch)
            history["val_auc_agnostic"].append(aucs["agnostic"])
            history["val_auc_wc"].append(aucs["wc"])
            history["val_auc_wa"].append(aucs["wa"])
            history["val_auc_wp"].append(aucs["wp"])
            print(f"  Agnostic (any) : {aucs['agnostic']:.1%}")
            print(f"  Wheelchair(wc) : {aucs['wc']:.1%}")
            print(f"  Walker    (wa) : {aucs['wa']:.1%}")
            print(f"  Person    (wp) : {aucs['wp']:.1%}")
            print()

    # Load best weights before saving final checkpoint
    if best_path is not None and best_path.exists():
        print(f"  Loading best checkpoint (val_loss={best_val_loss:.4f}) …")
        load_checkpoint(best_path, net)

    save_checkpoint(args.out, net, optimizer, epoch=final_epoch,
                    detector_name=args.detector, dataset_name=cfg.name)

    return history


# ---------------------------------------------------------------------------
# Hyperparameter tuning (Optuna)
# ---------------------------------------------------------------------------

def tune_model(args) -> dict:
    """
    Run an Optuna study to find the best hyperparameters for args.detector.

    Parameters
    ----------
    args : Namespace
        Base training args.  ``args.tune_trials`` and ``args.tune_epochs``
        control the study; all other args serve as defaults that are partially
        overridden inside each trial.

    Returns
    -------
    dict  Best hyperparameters found by Optuna.

    Notes
    -----
    Objective  — minimise the lowest val_loss reached in the trial.
                 Falls back to train_loss when no val split is available.
    Parameters tuned for every model:
        lr, weight_decay, dropout
    Additional parameters for full-scan models:
        backbone_channels, hidden          (fullscan_cnn / fullscan_transformer)
        out_channels                       (spacetime_cnn)
        n_heads                            (fullscan_transformer)
    """
    try:
        import optuna
    except ImportError:
        raise ImportError(
            "optuna is required for hyperparameter tuning.  "
            "Install it with: pip install optuna"
        )

    import tempfile

    optuna.logging.set_verbosity(optuna.logging.WARNING)

    det          = args.detector
    tune_epochs  = getattr(args, "tune_epochs",  10)
    tune_trials  = getattr(args, "tune_trials",  30)

    def objective(trial: "optuna.Trial") -> float:
        # Copy base args, then override with trial suggestions
        t = SimpleNamespace(**vars(args))
        t.lr           = trial.suggest_float("lr",           1e-5, 1e-2, log=True)
        t.weight_decay = trial.suggest_float("weight_decay", 1e-6, 1e-2, log=True)
        t.dropout      = trial.suggest_float("dropout",      0.0,  0.6,  step=0.1)

        if det in ("fullscan_cnn", "fullscan_transformer"):
            t.backbone_channels = trial.suggest_categorical(
                "backbone_channels", [32, 64, 128])
            t.hidden = trial.suggest_categorical(
                "hidden", [64, 128, 256])
        if det == "spacetime_cnn":
            t.out_channels = trial.suggest_categorical(
                "out_channels", [64, 128, 256])
        if det == "fullscan_transformer":
            t.n_heads = trial.suggest_categorical("n_heads", [4, 8])

        # GRU-based models cannot run on DirectML — force CPU inside trials
        t.force_cpu = det in ("fullscan_cnn", "fullscan_transformer")

        # Shorter run; skip AUC inside trials (expensive)
        t.epochs    = tune_epochs
        t.auc_every = 0
        t.patience  = max(3, tune_epochs // 3)

        # Temporary checkpoint files — cleaned up after the trial
        tmp = Path(tempfile.mktemp(suffix=".pth"))
        t.out = tmp

        try:
            history = train_model(t)
        finally:
            tmp.unlink(missing_ok=True)
            tmp.with_suffix(".best.pth").unlink(missing_ok=True)

        if history["val_loss"]:
            return min(history["val_loss"])
        return min(history["train_loss"])

    study = optuna.create_study(
        direction="minimize",
        study_name=f"tune_{det}",
        sampler=optuna.samplers.TPESampler(seed=42),
    )
    print(f"\nStarting Optuna study: {tune_trials} trials × {tune_epochs} epochs "
          f"each for '{det}' …\n")
    study.optimize(objective, n_trials=tune_trials, show_progress_bar=True)

    best = study.best_params
    print(f"\n{'='*60}")
    print(f"  Best val_loss : {study.best_value:.4f}")
    print(f"  Best params   :")
    for k, v in best.items():
        print(f"    {k} = {v}")
    print(f"{'='*60}\n")

    return best


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Train and evaluate person detectors on DROW or FROG.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    # Trainable detectors — DROW and DR-SPAAM use published weights, not trained here
    _TRAINABLE = ["algorithmic", "fullscan_cnn", "spacetime_cnn", "fullscan_transformer"]
    parser.add_argument(
        "--detector",
        choices=_TRAINABLE,
        default="fullscan_cnn",
        help="Detector to train/evaluate (default: fullscan_cnn)",
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
                        help="Dropout for all models (default: 0.5)")
    parser.add_argument("--time-frame",  type=int,   default=5,
                        help="Number of scans in the temporal window (default: 5)")
    parser.add_argument("--vote-radius", type=float, default=0.6)
    parser.add_argument("--vote-weight", type=float, default=0.02)
    parser.add_argument("--subsample",   type=float, default=1.0,
                        help="Fraction of frames per epoch (use 0.05 for quick dev)")
    parser.add_argument("--batch-size",  type=int,   default=4,
                        help="Frames per optimizer step; effective beam batch = "
                             "batch_size × N_beams (default: 4)")
    # Architecture hyperparams (full-scan models only)
    parser.add_argument("--backbone-channels", type=int, default=64,
                        help="Conv channels in DilatedScanBackbone "
                             "(FullScanCNN/Transformer; default: 64)")
    parser.add_argument("--hidden",      type=int,   default=128,
                        help="GRU hidden size (FullScanCNN/Transformer; default: 128)")
    parser.add_argument("--n-heads",     type=int,   default=8,
                        help="Attention heads (FullScanTransformer; default: 8)")
    parser.add_argument("--out-channels",type=int,   default=128,
                        help="Output channels (SpaceTimeCNN; default: 128)")
    # Regularisation / scheduling
    parser.add_argument("--patience",    type=int,   default=0,
                        help="Early-stopping patience in epochs (0=disabled). "
                             "Saves best checkpoint to <out>.best.pth.")
    parser.add_argument("--lr-schedule", choices=["none", "cosine", "plateau"],
                        default="none",
                        help="LR schedule: none | cosine | plateau (default: none)")
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
    # Hyperparameter tuning (Optuna)
    parser.add_argument("--tune",        action="store_true",
                        help="Run Optuna hyperparameter search instead of training. "
                             "Requires: pip install optuna")
    parser.add_argument("--tune-trials", type=int, default=30,
                        help="Number of Optuna trials (default: 30)")
    parser.add_argument("--tune-epochs", type=int, default=10,
                        help="Epochs per trial — keep short (default: 10)")
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
        print("(Algorithmic evaluation is available via evaluate.py --algo)")
        return

    # ------------------------------------------------------------------
    # Eval-only mode
    # ------------------------------------------------------------------
    if args.eval_only:
        device, using_dml = detect_device()
        train_ds, val_ds, cfg = _setup_datasets(args)
        net = _build_model(args).to(device)
        if args.weights:
            load_checkpoint(args.weights, net)
        elif args.resume:
            load_checkpoint(args.resume, net)
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
    # Hyperparameter tuning
    # ------------------------------------------------------------------
    if args.tune:
        tune_model(args)
        return

    # ------------------------------------------------------------------
    # Training
    # ------------------------------------------------------------------
    # force_cpu is not a CLI flag — it's only used programmatically.
    # Attach the default so train_model() doesn't choke on getattr.
    args.force_cpu = False
    train_model(args)


if __name__ == "__main__":
    main()
