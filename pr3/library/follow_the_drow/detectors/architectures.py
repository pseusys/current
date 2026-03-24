"""
Neural network architectures for lidar-based person detection.

References:
  DROW:     Beyer et al., IROS 2018  — arXiv:1603.02636
  DR-SPAAM: Jia et al.,  RA-L  2022 — arXiv:2004.14064
"""

import math
from pathlib import Path
from typing import Optional, Tuple

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F

from ..utils.file_utils import DRSPAAM_WEIGHTS_PATH

_DRSPAAM_WEIGHTS_PATH = Path(__file__).parent.parent / DRSPAAM_WEIGHTS_PATH


# ---------------------------------------------------------------------------
# Beam spatial attention (used by FullScanTransformerDetector)
# ---------------------------------------------------------------------------

class BeamSelfAttention(nn.Module):
    """
    Global multi-head self-attention over the beam (angular) dimension.

    Every beam attends to every other beam — content-adaptive, global communication
    across the full scan. Applied independently at each of the T timesteps
    (T acts as the batch dimension).

    At N_beams=450 with 8 heads: attention tensors ≈ 3 MB per layer — feasible
    on GPU; slow on CPU (~1.5 s/scan).

    Input / output: (N_beams, T, C)  — same shape, pre-norm residual included.
    """

    def __init__(self, channels: int, n_heads: int = 8, dropout: float = 0.1):
        super().__init__()
        self.attn = nn.MultiheadAttention(channels, n_heads,
                                          dropout=dropout, batch_first=True)
        self.norm = nn.LayerNorm(channels)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # x: (N_beams, T, C)
        # Transpose: T acts as batch, N_beams as the attention sequence.
        x_t = x.permute(1, 0, 2)           # (T, N_beams, C)
        out, _ = self.attn(x_t, x_t, x_t)  # (T, N_beams, C)
        x_t = self.norm(x_t + out)          # pre-norm residual
        return x_t.permute(1, 0, 2)         # (N_beams, T, C)


# ---------------------------------------------------------------------------
# DR-SPAAM building blocks — faithful to the official implementation
# ---------------------------------------------------------------------------

def _conv3x3(in_channel: int, out_channel: int) -> nn.Sequential:
    """Conv1d(k=3, pad=1) + BN + LeakyReLU — the basic DR-SPAAM building block."""
    return nn.Sequential(
        nn.Conv1d(in_channel, out_channel, kernel_size=3, padding=1),
        nn.BatchNorm1d(out_channel),
        nn.LeakyReLU(negative_slope=0.1, inplace=True),
    )


class _SpatialAttention(nn.Module):
    """
    Auto-regressive spatial attention from DR-SPAAM (Jia et al., RA-L 2022).

    Matches ``_SpatialAttention`` in the official DR-SPAAM-Detector repo.
    For each beam, attends to its ``window_size`` nearest angular neighbours
    in a *template* feature map from the previous timestep, blending them
    with weight ``alpha``.

    Input:  x, x_template — both (B, N_beams, C, n_pts)
    Output: (out_temp, sim) — updated template (B, N_beams, C, n_pts) and
                              attention weights (B, N_beams, N_beams)
    """

    def __init__(self, n_pts: int, n_channel: int = 256,
                 alpha: float = 0.5, window_size: int = 11):
        super().__init__()
        self._alpha       = alpha
        self._window_size = window_size
        # Collapses the n_pts spatial dimension into a 128-d beam embedding
        self.conv = nn.Sequential(
            nn.Conv1d(n_channel, 128, kernel_size=n_pts),
            nn.BatchNorm1d(128),
            nn.LeakyReLU(negative_slope=0.1, inplace=True),
        )
        self._neighbor_mask: Optional[torch.Tensor] = None

        for m in self.modules():
            if isinstance(m, nn.Conv1d):
                nn.init.kaiming_normal_(m.weight, a=0.1, nonlinearity="leaky_relu")
                if m.bias is not None:
                    nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.BatchNorm1d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def _make_mask(self, n: int, device: torch.device) -> torch.Tensor:
        hw  = self._window_size // 2
        idx = torch.arange(n, device=device)
        col = (idx.unsqueeze(-1)
               + torch.arange(-hw, hw + 1, device=device)).clamp(0, n - 1)
        row = idx.unsqueeze(-1).expand_as(col)
        mask = torch.zeros(n, n, device=device)
        mask[row.reshape(-1), col.reshape(-1)] = 1.0
        return mask

    def forward(self, x: torch.Tensor,
                x_template: torch.Tensor) -> Tuple[torch.Tensor, torch.Tensor]:
        B, N, C, n_pts = x.shape
        if (self._neighbor_mask is None
                or self._neighbor_mask.shape[0] != N
                or self._neighbor_mask.device != x.device):
            self._neighbor_mask = self._make_mask(N, x.device)

        emb_x    = self.conv(x.reshape(B * N, C, n_pts)).reshape(B, N, 128)
        emb_temp = self.conv(x_template.reshape(B * N, C, n_pts)).reshape(B, N, 128)

        sim  = torch.matmul(emb_x, emb_temp.permute(0, 2, 1))       # (B, N, N)
        sim  = sim - 1e10 * (1.0 - self._neighbor_mask)              # mask non-nbrs
        maxv = sim.max(dim=-1, keepdim=True)[0]
        exps = torch.exp(sim - maxv) * self._neighbor_mask
        sim  = exps / (exps.sum(dim=-1, keepdim=True) + 1e-8)        # (B, N, N)

        out_temp = torch.matmul(sim, x_template.reshape(B, N, C * n_pts))
        out_temp = out_temp.reshape(B, N, C, n_pts)
        return self._alpha * x + (1.0 - self._alpha) * out_temp, sim


# ---------------------------------------------------------------------------
# DR-SPAAM detector — faithful to SpatialDROW in the official repo
# ---------------------------------------------------------------------------

class DrSpaamDetector(nn.Module):
    """
    DR-SPAAM person detector — faithful to Jia et al., RA-L 2022 (arXiv:2004.14064).

    Matches the official SpatialDROW implementation at:
      https://github.com/VisualComputingInstitute/DR-SPAAM-Detector

    Architecture
    ------------
    Four 1-D conv blocks (nested Sequential of _conv3x3 layers, producing the
    same state-dict key structure as the official checkpoint):
      Block 1  (1→128,  3×_conv3x3) + MaxPool(2)
      Block 2  (128→256, 3×_conv3x3) + MaxPool(2)  ← spatial attention here
      Block 3  (256→512, 3×_conv3x3) + MaxPool(2)
      Block 4  (512→128, 2×_conv3x3) + AvgPool(n_pts//8)
    Conv1d output heads: conv_cls (n_cls) and conv_reg (2).

    Spatial attention: auto-regressive template updated scan-by-scan within the
    T-scan window; each beam attends to ±window_size//2 angular neighbours in
    the previous template.

    Input / output
    --------------
    Single scan (eval):   x (N_beams, T, S) → logits (N_beams, n_cls), votes (N_beams, 2)
    Batched (training):   x (B, N_beams, T, S) → logits (B*N, n_cls),  votes (B*N, 2)

    n_cls = 1 for pedestrian_only (published weights) or 4 when training.

    BEAM_BATCH = True signals train.py to pass (B, N, T, S) so that all N_beams
    of one scan stay together for the spatial attention.
    """

    INPUT_MODE = "cutout"
    N_SAMP     = 56       # published weights use 56-pt cutouts (RA-L 2022)
    BEAM_BATCH = True     # train.py: pass (B, N, T, S), not flattened (B*N, T, S)
    DEFAULT_WEIGHTS = _DRSPAAM_WEIGHTS_PATH

    def __init__(self, dropout: float = 0.5, num_scans: int = 5,
                 num_pts: int = 56, alpha: float = 0.5,
                 window_size: int = 11, pedestrian_only: bool = False,
                 verbose: bool = False):
        super().__init__()
        self.dropout         = dropout
        self.num_scans       = num_scans
        self.num_pts         = num_pts
        self.pedestrian_only = pedestrian_only
        self._verbose        = verbose
        # Spatial size after two MaxPool(2) — matches gate conv kernel_size
        self._n_pts          = num_pts // 4

        # Four conv blocks.  Each _conv3x3 is itself a Sequential, so the
        # nested structure produces keys like conv_block_1.0.0.weight which
        # matches the official DR-SPAAM checkpoint exactly.
        self.conv_block_1 = nn.Sequential(
            _conv3x3(1,   64),
            _conv3x3(64,  64),
            _conv3x3(64,  128),
        )
        self.conv_block_2 = nn.Sequential(
            _conv3x3(128, 128),
            _conv3x3(128, 128),
            _conv3x3(128, 256),
        )
        self.conv_block_3 = nn.Sequential(
            _conv3x3(256, 256),
            _conv3x3(256, 256),
            _conv3x3(256, 512),
        )
        self.conv_block_4 = nn.Sequential(
            _conv3x3(512, 256),
            _conv3x3(256, 128),
        )

        n_cls = 1 if pedestrian_only else 4
        self.conv_cls = nn.Conv1d(128, n_cls, kernel_size=1)
        self.conv_reg = nn.Conv1d(128, 2,     kernel_size=1)

        self.gate = _SpatialAttention(
            n_pts=self._n_pts, n_channel=256,
            alpha=alpha, window_size=window_size,
        )

        self._reset_parameters()

    def _reset_parameters(self) -> None:
        for m in self.modules():
            if isinstance(m, nn.Conv1d):
                nn.init.kaiming_normal_(m.weight, a=0.1, nonlinearity="leaky_relu")
                if m.bias is not None:
                    nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.BatchNorm1d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)
        # Zero-init output heads for a stable training start
        nn.init.constant_(self.conv_cls.weight, 0)
        if self.conv_cls.bias is not None:
            nn.init.constant_(self.conv_cls.bias, 0)
        nn.init.constant_(self.conv_reg.weight, 0)
        if self.conv_reg.bias is not None:
            nn.init.constant_(self.conv_reg.bias, 0)

    def _encode(self, x: torch.Tensor) -> torch.Tensor:
        """Encode (B*N, 1, S) → (B*N, 256, n_pts) through blocks 1–2."""
        x = F.max_pool1d(self.conv_block_1(x), 2)
        x = F.dropout(x, p=self.dropout, training=self.training)
        x = F.max_pool1d(self.conv_block_2(x), 2)
        x = F.dropout(x, p=self.dropout, training=self.training)
        return x

    def forward(self, x: torch.Tensor) -> Tuple[torch.Tensor, torch.Tensor]:
        """
        x : (N_beams, T, S)        single scan — eval / forward_one
          | (B, N_beams, T, S)    batch of scans — training
        Returns (logits, votes) always with B*N_beams on the first dimension.
        """
        single = (x.ndim == 3)
        if single:
            x = x.unsqueeze(0)           # (1, N, T, S)

        B, N, T, S = x.shape

        # Encode first scan → initial template  (B, N, 256, n_pts)
        x0       = x[:, :, 0, :].reshape(B * N, 1, S)
        template = self._encode(x0).reshape(B, N, 256, self._n_pts).detach()

        # Auto-regressive spatial attention over remaining scans.
        # The incoming template is already detached (stop-gradient on the
        # recurrent chain).  We keep the last output connected to the
        # computation graph so that gradients reach encode/gate on the
        # final step; intermediate outputs are detached to prevent BPTT.
        for t in range(1, T):
            xt   = x[:, :, t, :].reshape(B * N, 1, S)
            feat = self._encode(xt).reshape(B, N, 256, self._n_pts)
            template, _ = self.gate(feat, template)
            if t < T - 1:
                template = template.detach()

        # Decode final template through blocks 3–4
        out = template.reshape(B * N, 256, self._n_pts)
        out = F.max_pool1d(self.conv_block_3(out), 2)
        out = F.dropout(out, p=self.dropout, training=self.training)
        out = F.avg_pool1d(self.conv_block_4(out), self._n_pts // 2)

        logits = self.conv_cls(out)[:, :, 0]   # (B*N, n_cls)
        votes  = self.conv_reg(out)[:, :, 0]   # (B*N, 2)

        if single:
            n_cls = logits.shape[-1]
            return logits.reshape(N, n_cls), votes.reshape(N, 2)
        return logits, votes

    @torch.no_grad()
    def forward_one(self, xb) -> Tuple[np.ndarray, np.ndarray]:
        """Numpy-in, numpy-out inference (single scan)."""
        self.eval()
        x = torch.from_numpy(np.asarray(xb, dtype=np.float32))
        logits, votes = self(x)
        if self.pedestrian_only:
            # 1-class sigmoid output → expand to 4-class format (wp slot = class 3)
            prob  = torch.sigmoid(logits)           # (N, 1)
            confs = torch.zeros(logits.shape[0], 4)
            confs[:, 3] = prob[:, 0]
            return confs.cpu().numpy(), votes.cpu().numpy()
        return F.softmax(logits, dim=-1).cpu().numpy(), votes.cpu().numpy()

    def forward_all(self, va) -> Tuple[np.ndarray, np.ndarray]:
        """Inference over every annotated frame in dataset *va*."""
        from time import time
        from tqdm.auto import trange
        from numpy import array
        from ..utils.drow_utils import cutout as _cutout

        times, all_confs, all_votes = [], [], []
        for iseq in trange(len(va.det_id), desc="Sequences",
                           disable=not self._verbose):
            for idet in trange(len(va.det_id[iseq]), desc="Scans",
                               disable=not self._verbose, leave=False):
                t0    = time()
                iscan = va.idet2iscan[iseq][idet]
                scans, odoms = va.get_scan(iseq, iscan, self.num_scans)
                cut = _cutout(scans, odoms, len(va.scans[iseq][iscan]),
                              nsamp=self.N_SAMP)
                confs, votes = self.forward_one(cut)
                all_confs.append(confs)
                all_votes.append(votes)
                times.append(time() - t0)
        if self._verbose:
            print(f"Average detection time: {sum(times)/len(times):.4f} s")
        return array(all_confs), array(all_votes)

    def save_weights(self, path, dataset: str = "") -> None:
        """Save a checkpoint that can be reloaded by load()."""
        torch.save({
            "type":            "DrSpaamDetector",
            "num_scans":       self.num_scans,
            "num_pts":         self.num_pts,
            "dropout":         self.dropout,
            "alpha":           self.gate._alpha,
            "window_size":     self.gate._window_size,
            "pedestrian_only": self.pedestrian_only,
            "model":           self.state_dict(),
            "dataset":         dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "DrSpaamDetector":
        """Load a checkpoint saved by save_weights()."""
        ckpt  = torch.load(path, map_location=map_location)
        model = cls(
            num_scans       = ckpt.get("num_scans",       5),
            num_pts         = ckpt.get("num_pts",         56),
            dropout         = ckpt.get("dropout",         0.5),
            alpha           = ckpt.get("alpha",           0.5),
            window_size     = ckpt.get("window_size",     11),
            pedestrian_only = ckpt.get("pedestrian_only", False),
        )
        model.load_state_dict(ckpt["model"])
        return model

    @classmethod
    def load_published(cls, path=None,
                       map_location: str = "cpu") -> "DrSpaamDetector":
        """
        Load the official DR-SPAAM published weights (dr_spaam_e40.pth).

        Downloaded at pip-install time via setup.py; bundled as DEFAULT_WEIGHTS.
        Published weights: num_pts=56, pedestrian_only=True, window_size=11.
        """
        weights = path or cls.DEFAULT_WEIGHTS
        model   = cls(num_pts=56, pedestrian_only=True,
                      window_size=11, alpha=0.5)
        ckpt    = torch.load(weights, map_location=map_location)
        model.load_state_dict(ckpt["model_state"])   # official checkpoint key
        return model
