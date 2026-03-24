"""
Full-scan lidar-based person detectors.

These detectors process the *entire* scan at once, using the full angular
context available across all N_beams, in contrast to the per-beam cutout
approach used by DROW and DR-SPAAM.

All three share the same input/output contract as DrowDetector:
  forward(x)       → (logits (N_beams, 4), votes (N_beams, 2))
  forward_one(xb)  → (confs, votes)   numpy in / numpy out

Input shape: (N_beams, T, 3) where the 3 channels are (r, x, y):
  r: range measurement in metres
  x = -r·sin(φ): Cartesian, left of robot
  y =  r·cos(φ): Cartesian, forward of robot
Prepared via drow_utils.aligned_scan_xyz().

Detectors
---------
  FullScanCNNDetector         — Dilated 1D CNN over N_beams + GRU over T
  SpaceTimeCNNDetector        — 2D dilated Conv over (N_beams, T) grid, collapses T
  FullScanTransformerDetector — Dilated 1D CNN + beam-PE + BeamSelfAttention + temporal mean-pool

Receptive field (DilatedScanBackbone)
--------------------------------------
Four dilated Conv1d layers with dilation=1,2,4,8 and kernel_size=3 give a
total receptive field of 31 beams.  At DROW's 0.5°/beam resolution this covers
±7.5°.  At 3 m range that corresponds to ~0.8 m lateral extent — sufficient
to cover a standing person (shoulder width ~0.5 m).

Receptive field (SpaceTimeCNNDetector)
--------------------------------------
Three dilated Conv2d layers with beam-axis dilations 1,2,4 and kernel_size=3
give a total beam receptive field of 15 beams (±3.75° at 0.5°/beam).
"""

import math

import torch
import torch.nn as nn
import torch.nn.functional as F

from .architectures import BeamSelfAttention


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _make_sinusoidal_pe(max_len: int, d_model: int) -> torch.Tensor:
    """Sinusoidal positional encoding for beam index (beam 0 … max_len-1)."""
    pe       = torch.zeros(max_len, d_model)
    position = torch.arange(max_len, dtype=torch.float).unsqueeze(1)
    div_term = torch.exp(
        torch.arange(0, d_model, 2, dtype=torch.float) * (-math.log(10000.0) / d_model)
    )
    pe[:, 0::2] = torch.sin(position * div_term)
    pe[:, 1::2] = torch.cos(position * div_term)
    return pe


# ---------------------------------------------------------------------------
# Shared dilated backbone
# ---------------------------------------------------------------------------

class DilatedScanBackbone(nn.Module):
    """
    Dilated 1D CNN applied over the beam (angular) dimension.

    Processes the full scan with an exponentially growing receptive field
    (dilation = 1, 2, 4, 8).  Each timestep is treated independently as a
    batch element.

    Input:  (N_beams, T, in_channels)
    Output: (N_beams, T, out_channels)
    """

    def __init__(self, in_channels: int = 3, out_channels: int = 64,
                 dropout: float = 0.1):
        super().__init__()
        self.dropout_p   = dropout
        mid              = out_channels

        self.conv1 = nn.Conv1d(in_channels, mid, 3, padding=1,  dilation=1)
        self.bn1   = nn.BatchNorm1d(mid)
        self.conv2 = nn.Conv1d(mid,         mid, 3, padding=2,  dilation=2)
        self.bn2   = nn.BatchNorm1d(mid)
        self.conv3 = nn.Conv1d(mid,         mid, 3, padding=4,  dilation=4)
        self.bn3   = nn.BatchNorm1d(mid)
        self.conv4 = nn.Conv1d(mid,         mid, 3, padding=8,  dilation=8)
        self.bn4   = nn.BatchNorm1d(mid)
        # 1×1 projection for the end-to-end residual skip connection
        self.skip  = nn.Conv1d(in_channels, out_channels, 1, bias=False)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # x: (N_beams, T, in_channels)
        N, T, C = x.shape
        x_t = x.permute(1, 2, 0)                           # (T, in_channels, N_beams)
        res = self.skip(x_t)                                # (T, out_channels, N_beams)

        x_t = F.relu(self.bn1(self.conv1(x_t)))
        x_t = F.dropout(x_t, p=self.dropout_p, training=self.training)
        x_t = F.relu(self.bn2(self.conv2(x_t)))
        x_t = F.dropout(x_t, p=self.dropout_p, training=self.training)
        x_t = F.relu(self.bn3(self.conv3(x_t)))
        x_t = F.dropout(x_t, p=self.dropout_p, training=self.training)
        x_t = F.relu(self.bn4(self.conv4(x_t)) + res)      # residual before final activation

        return x_t.permute(2, 0, 1)                        # (N_beams, T, out_channels)


# ---------------------------------------------------------------------------
# Full-scan detectors
# ---------------------------------------------------------------------------

class FullScanCNNDetector(nn.Module):
    """
    Full-scan detector: dilated 1D CNN over beams + GRU over time.

    Architecture (Option A from DISCOVERY.md):
      DilatedScanBackbone(3 → 64)  →  GRU(64 → hidden)  →  detection heads

    The GRU processes T=5 timesteps per beam and returns the final hidden state,
    capturing temporal motion cues (approach/retreat) without making assumptions
    about the temporal ordering beyond sequence direction.
    """

    INPUT_MODE = "full_scan"

    def __init__(self, n_time: int = 5, backbone_channels: int = 64,
                 hidden: int = 128, dropout: float = 0.1):
        super().__init__()
        self.n_time   = n_time
        self.backbone = DilatedScanBackbone(3, backbone_channels, dropout)
        self.gru      = nn.GRU(backbone_channels, hidden, batch_first=True)
        self.head_logits = nn.Linear(hidden, 4)
        self.head_votes  = nn.Linear(hidden, 2)

    def forward(self, x: torch.Tensor):
        """x : (N_beams, T, 3)"""
        feats  = self.backbone(x)          # (N_beams, T, backbone_channels)
        _, h   = self.gru(feats)           # h: (1, N_beams, hidden)
        pooled = h.squeeze(0)              # (N_beams, hidden)
        return self.head_logits(pooled), self.head_votes(pooled)

    @torch.no_grad()
    def forward_one(self, xb) -> tuple:
        """Numpy in, numpy out.  xb: (N_beams, T, 3)"""
        import numpy as np
        self.eval()
        x = torch.from_numpy(np.asarray(xb, dtype=np.float32))
        logits, votes = self(x)
        return F.softmax(logits, dim=-1).cpu().numpy(), votes.cpu().numpy()

    def save_weights(self, path, dataset: str = ""):
        torch.save({
            "type":              "FullScanCNNDetector",
            "n_time":            self.n_time,
            "backbone_channels": self.backbone.conv4.out_channels,
            "hidden":            self.gru.hidden_size,
            "model":             self.state_dict(),
            "dataset":           dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "FullScanCNNDetector":
        ckpt  = torch.load(path, map_location=map_location)
        model = cls(
            n_time=ckpt.get("n_time", 5),
            backbone_channels=ckpt.get("backbone_channels", 64),
            hidden=ckpt.get("hidden", 128),
        )
        model.load_state_dict(ckpt["model"])
        return model


# ---------------------------------------------------------------------------
# SpaceTimeCNNDetector building blocks
# ---------------------------------------------------------------------------

def _gn(num_channels: int) -> nn.GroupNorm:
    """GroupNorm selecting the largest power-of-2 divisor of num_channels."""
    for g in (32, 16, 8, 4, 2, 1):
        if num_channels % g == 0:
            return nn.GroupNorm(g, num_channels)


class _SEBlock(nn.Module):
    """Squeeze-and-Excitation channel attention (Hu et al., CVPR 2018).

    Global-averages the spatial map, passes through a 2-layer bottleneck MLP,
    and re-weights each channel with a sigmoid gate.  Near-zero parameter
    overhead; consistently adds ~1 % accuracy in image CNNs.

    Input / output: (B, C, H, W) — same shape.
    """

    def __init__(self, channels: int, reduction: int = 8):
        super().__init__()
        mid = max(channels // reduction, 4)
        self.fc = nn.Sequential(
            nn.Linear(channels, mid, bias=False),
            nn.ReLU(inplace=True),
            nn.Linear(mid, channels, bias=False),
            nn.Sigmoid(),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        w = x.mean(dim=(2, 3))                       # (B, C) global avg-pool
        w = self.fc(w)                               # (B, C) per-channel gate
        return x * w.unsqueeze(-1).unsqueeze(-1)


class _JointConvBlock(nn.Module):
    """Dilated 2-D conv block over both beam and time axes simultaneously.

    Captures joint space-time patterns (e.g. a range-decrease at a specific
    angular position over consecutive frames) that purely factorised
    beam-then-time towers cannot represent.

    For k=3 and dilation d the same-padding is d on each axis.

    Input / output: (1, C, N_beams, T) — same shape.
    """

    def __init__(self, C_in: int, C_out: int,
                 dilation: tuple = (1, 1), dropout: float = 0.1):
        super().__init__()
        pad            = (dilation[0], dilation[1])   # same-padding for k=3
        self.conv      = nn.Conv2d(C_in, C_out, (3, 3),
                                   padding=pad, dilation=dilation, bias=False)
        self.norm      = _gn(C_out)
        self.skip      = (nn.Conv2d(C_in, C_out, 1, bias=False)
                          if C_in != C_out else nn.Identity())
        self.dropout_p = dropout

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        out = F.relu(self.norm(self.conv(x)))
        out = F.dropout(out, p=self.dropout_p, training=self.training)
        return out + self.skip(x)


class _MultiScaleBlock(nn.Module):
    """Inception-style multi-scale conv block for one axis (beam or time).

    Runs parallel branches with different kernel sizes along the specified
    axis, concatenates the outputs and projects back to C_out via a 1×1 conv,
    then applies a Squeeze-and-Excitation gate and a residual skip.

    axis='beam'  — kernels are (k, 1) with beam-axis dilation; the T dimension
                   passes through unmodified so subsequent temporal stages see
                   the full T.
    axis='time'  — kernels are (1, k); beam axis is unchanged.

    Stacking two of these blocks for the same axis builds a compositional
    hierarchy: Stage A learns velocity-level patterns; Stage B (applied to
    Stage A's output) learns acceleration / periodicity — patterns that no
    single wide kernel can represent alone.

    Input / output: (1, C, N_beams, T) — same shape.
    """

    def __init__(self, C_in: int, C_out: int, kernel_sizes: tuple,
                 axis: str, dilation: int = 1, dropout: float = 0.1):
        super().__init__()
        assert axis in ('beam', 'time')
        n        = len(kernel_sizes)
        C_branch = C_out // n            # channels per parallel branch

        self.branches = nn.ModuleList()
        for k in kernel_sizes:
            if axis == 'beam':
                pad  = ((k - 1) // 2 * dilation, 0)
                kern = (k, 1)
                dil  = (dilation, 1)
            else:                        # time
                pad  = (0, (k - 1) // 2)
                kern = (1, k)
                dil  = (1, 1)
            self.branches.append(nn.Sequential(
                nn.Conv2d(C_in, C_branch, kern,
                          padding=pad, dilation=dil, bias=False),
                _gn(C_branch),
                nn.ReLU(inplace=True),
            ))

        self.project = nn.Sequential(
            nn.Conv2d(C_branch * n, C_out, 1, bias=False),
            _gn(C_out),
        )
        self.se        = _SEBlock(C_out)
        self.skip      = (nn.Conv2d(C_in, C_out, 1, bias=False)
                          if C_in != C_out else nn.Identity())
        self.dropout_p = dropout

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        out = torch.cat([b(x) for b in self.branches], dim=1)
        out = F.relu(self.project(out))
        out = F.dropout(out, p=self.dropout_p, training=self.training)
        out = self.se(out)
        return out + self.skip(x)


# ---------------------------------------------------------------------------
# SpaceTimeCNNDetector
# ---------------------------------------------------------------------------

class SpaceTimeCNNDetector(nn.Module):
    """
    Full-scan detector: multi-scale 2-D conv over the (N_beams × T) space-time grid.

    Architecture
    ------------
    Input (N_beams, T, 3) is reshaped to (1, 3, N_beams, T) and processed as
    a 2-D "image" where height = beams and width = time frames.

    Stem
      A single joint Conv2d(3×3) to lift the 3 input channels to C.

    Joint blocks (2×, dilations 1 and 2 on both axes)
      Dilated 2-D convolutions that learn *co-occurring* beam+time patterns —
      e.g. "range is falling specifically at beam 200 over three frames" — which
      purely factorised spatial-then-temporal towers cannot represent.

    Spatial stages (n_spatial_stages = 1–4, default 3)
      Stacked _MultiScaleBlock along the beam axis.  Each stage runs parallel
      inception branches with kernel sizes (3, 5, 9) at an exponentially
      growing beam dilation (1 → 4 → 16 → 64).
        • narrow branches (k=3):  person features at far range (few beams)
        • wide branches (k=9):    person features at close range (many beams)
      Stacking builds hierarchy — Stage A detects leg-scale clusters, Stage B
      composes them into person-scale features, and so on.

    Temporal stages (2×, kernel sizes 3, 5, 7)
      Stacked _MultiScaleBlock along the time axis.  Stage A sees the raw
      temporal signal at three scales (velocity-like); Stage B operates on
      Stage A's output to learn acceleration / gait-periodicity patterns.

    Collapse
      Mean over the T axis — handles any T without architecture changes.

    Beam receptive field (widest branch k=9 at each spatial stage)
    ---------------------------------------------------------------
    pre-spatial (stem + 2 joint blocks):  9 beams
    + Spatial A  (d=1):                  17 beams
    + Spatial B  (d=4):                  49 beams
    + Spatial C  (d=16):                177 beams   ← default (n_spatial_stages=3)
    + Spatial D  (d=64):                689 beams   ← optional (n_spatial_stages=4)

    At 0.5°/beam: 3 stages → ±44°; 4 stages → covers full 720-beam FROG scan.

    DirectML-compatible (no GRU).
    """

    INPUT_MODE = "full_scan"

    _SPATIAL_DILATIONS = (1, 4, 16, 64)
    _SPATIAL_KS        = (3, 5, 9)
    _TEMPORAL_KS       = (3, 5, 7)

    def __init__(self, n_time: int = 5, channels: int = 96,
                 n_spatial_stages: int = 3, dropout: float = 0.1):
        super().__init__()
        assert 1 <= n_spatial_stages <= 4, "n_spatial_stages must be 1–4"
        self.n_time           = n_time
        self.channels         = channels
        self.n_spatial_stages = n_spatial_stages
        C = channels

        # ── Stem ────────────────────────────────────────────────────────────
        self.stem = nn.Sequential(
            nn.Conv2d(3, C, (3, 3), padding=(1, 1), bias=False),
            _gn(C),
            nn.ReLU(inplace=True),
        )

        # ── Joint blocks ─────────────────────────────────────────────────────
        self.joint1 = _JointConvBlock(C, C, dilation=(1, 1), dropout=dropout)
        self.joint2 = _JointConvBlock(C, C, dilation=(2, 2), dropout=dropout)

        # ── Spatial multi-scale stages ───────────────────────────────────────
        self.spatial_stages = nn.ModuleList([
            _MultiScaleBlock(C, C, self._SPATIAL_KS, axis='beam',
                             dilation=d, dropout=dropout)
            for d in self._SPATIAL_DILATIONS[:n_spatial_stages]
        ])

        # ── Temporal multi-scale stages ──────────────────────────────────────
        self.temporal_stages = nn.ModuleList([
            _MultiScaleBlock(C, C, self._TEMPORAL_KS, axis='time', dropout=dropout),
            _MultiScaleBlock(C, C, self._TEMPORAL_KS, axis='time', dropout=dropout),
        ])

        self.head_logits = nn.Linear(C, 4)
        self.head_votes  = nn.Linear(C, 2)

    def forward(self, x: torch.Tensor):
        """x : (N_beams, T, 3)"""
        x = x.permute(2, 0, 1).unsqueeze(0)   # → (1, 3, N_beams, T)

        x = self.stem(x)
        x = self.joint1(x)
        x = self.joint2(x)

        for stage in self.spatial_stages:
            x = stage(x)

        for stage in self.temporal_stages:
            x = stage(x)

        x = x.mean(dim=-1)                     # (1, C, N_beams) — collapse T
        x = x.squeeze(0).permute(1, 0)         # (N_beams, C)
        return self.head_logits(x), self.head_votes(x)

    @torch.no_grad()
    def forward_one(self, xb) -> tuple:
        """Numpy in, numpy out.  xb: (N_beams, T, 3)"""
        import numpy as np
        self.eval()
        x = torch.from_numpy(np.asarray(xb, dtype=np.float32))
        logits, votes = self(x)
        return F.softmax(logits, dim=-1).cpu().numpy(), votes.cpu().numpy()

    def save_weights(self, path, dataset: str = ""):
        torch.save({
            "type":             "SpaceTimeCNNDetector",
            "n_time":           self.n_time,
            "channels":         self.channels,
            "n_spatial_stages": self.n_spatial_stages,
            "model":            self.state_dict(),
            "dataset":          dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "SpaceTimeCNNDetector":
        ckpt     = torch.load(path, map_location=map_location)
        channels = ckpt.get("channels", ckpt.get("out_channels", 96))
        model    = cls(
            n_time           = ckpt.get("n_time", 5),
            channels         = channels,
            n_spatial_stages = ckpt.get("n_spatial_stages", 3),
        )
        model.load_state_dict(ckpt["model"])
        return model


class FullScanTransformerDetector(nn.Module):
    """
    Full-scan detector: dilated 1D CNN + sinusoidal beam PE + global beam self-attention
    + temporal mean-pool.

    Architecture (Option C from DISCOVERY.md, updated):
      DilatedScanBackbone(3 → backbone_channels)
        → add sinusoidal beam-index positional encoding
        → BeamSelfAttention(backbone_channels, n_heads)   [O(N_beams²) per step]
        → temporal mean-pool over T
        → detection heads

    Changes from original design
    ----------------------------
    * Sinusoidal beam-angle PE injected before attention so the model knows
      which angular position each beam occupies (was permutation-invariant before).
    * GRU replaced by temporal mean-pool: at T=5 the recurrence adds negligible
      benefit while preventing DirectML usage and dominating training cost.

    Note: at N_beams=450 with 8 heads the attention tensor is ~3 MB/layer.
    Feasible on GPU; ~10 ms/scan on CPU.
    DirectML-compatible (no GRU).
    """

    INPUT_MODE = "full_scan"

    def __init__(self, n_time: int = 5, backbone_channels: int = 64,
                 n_heads: int = 8, dropout: float = 0.1):
        super().__init__()
        self.n_time    = n_time
        self.backbone  = DilatedScanBackbone(3, backbone_channels, dropout)
        self.beam_attn = BeamSelfAttention(backbone_channels, n_heads, dropout)
        # Sinusoidal positional encoding indexed by beam number (up to 1024 beams)
        self.register_buffer("_beam_pe", _make_sinusoidal_pe(1024, backbone_channels))
        self.head_logits = nn.Linear(backbone_channels, 4)
        self.head_votes  = nn.Linear(backbone_channels, 2)

    def forward(self, x: torch.Tensor):
        """x : (N_beams, T, 3)"""
        N      = x.shape[0]
        feats  = self.backbone(x)                           # (N_beams, T, backbone_channels)
        feats  = feats + self._beam_pe[:N].unsqueeze(1)     # inject beam-index PE
        feats  = self.beam_attn(feats)                      # (N_beams, T, backbone_channels)
        pooled = feats.mean(dim=1)                          # (N_beams, backbone_channels)
        return self.head_logits(pooled), self.head_votes(pooled)

    @torch.no_grad()
    def forward_one(self, xb) -> tuple:
        """Numpy in, numpy out.  xb: (N_beams, T, 3)"""
        import numpy as np
        self.eval()
        x = torch.from_numpy(np.asarray(xb, dtype=np.float32))
        logits, votes = self(x)
        return F.softmax(logits, dim=-1).cpu().numpy(), votes.cpu().numpy()

    def save_weights(self, path, dataset: str = ""):
        torch.save({
            "type":              "FullScanTransformerDetector",
            "n_time":            self.n_time,
            "backbone_channels": self.backbone.conv4.out_channels,
            "n_heads":           self.beam_attn.attn.num_heads,
            "model":             self.state_dict(),
            "dataset":           dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "FullScanTransformerDetector":
        ckpt  = torch.load(path, map_location=map_location)
        model = cls(
            n_time=ckpt.get("n_time", 5),
            backbone_channels=ckpt.get("backbone_channels", 64),
            n_heads=ckpt.get("n_heads", 8),
        )
        model.load_state_dict(ckpt["model"])
        return model
