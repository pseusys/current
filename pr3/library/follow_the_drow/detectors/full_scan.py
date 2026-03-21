"""
Full-scan lidar-based person detectors.

Unlike the cutout-based PersonDetector (which extracts a fixed polar window
around each beam), these detectors process the *entire* scan at once, using the
full angular context available across all N_beams.

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
  SpaceTimeCNNDetector        — 2D Conv over (N_beams, T) grid, collapses T
  FullScanTransformerDetector — Dilated 1D CNN + BeamSelfAttention + GRU over T

Receptive field (DilatedScanBackbone)
--------------------------------------
Four dilated Conv1d layers with dilation=1,2,4,8 and kernel_size=3 give a
total receptive field of 31 beams.  At DROW's 0.5°/beam resolution this covers
±7.5°.  At 3 m range that corresponds to ~0.8 m lateral extent — sufficient
to cover a standing person (shoulder width ~0.5 m).
"""

import torch
import torch.nn as nn
import torch.nn.functional as F

from .architectures import BeamSelfAttention


# ---------------------------------------------------------------------------
# Shared dilated backbone
# ---------------------------------------------------------------------------

class DilatedScanBackbone(nn.Module):
    """
    Dilated 1D CNN applied over the beam (angular) dimension.

    Processes the full scan with an exponentially growing receptive field
    (dilation = 1, 2, 4, 8).  Each timestep is treated independently as a
    batch element — identical to how BeamNeighborConv works in architectures.py.

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

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # x: (N_beams, T, in_channels)
        N, T, C = x.shape
        x_t = x.permute(1, 2, 0)                           # (T, in_channels, N_beams)

        x_t = F.relu(self.bn1(self.conv1(x_t)))
        x_t = F.dropout(x_t, p=self.dropout_p, training=self.training)
        x_t = F.relu(self.bn2(self.conv2(x_t)))
        x_t = F.dropout(x_t, p=self.dropout_p, training=self.training)
        x_t = F.relu(self.bn3(self.conv3(x_t)))
        x_t = F.dropout(x_t, p=self.dropout_p, training=self.training)
        x_t = F.relu(self.bn4(self.conv4(x_t)))

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


class SpaceTimeCNNDetector(nn.Module):
    """
    Full-scan detector: 2D convolution over the (N_beams, T) space-time grid.

    Architecture (Option B from DISCOVERY.md):
      (N_beams, T, 3) reshaped to (1, 3, N_beams, T) as a 2D "image".
      Two Conv2d layers with (3×3) kernels enrich both angular and temporal
      context simultaneously.  A final (3×T) Conv2d collapses the time axis,
      producing per-beam features that feed the detection heads.

    Unlike the CNN+GRU approach this model has no directional temporal bias —
    it treats all T frames symmetrically, which may help for static people.
    """

    INPUT_MODE = "full_scan"

    def __init__(self, n_time: int = 5, out_channels: int = 128,
                 dropout: float = 0.1):
        super().__init__()
        self.n_time    = n_time
        self.dropout_p = dropout

        self.conv1 = nn.Conv2d(3,   64,          (3, 3),      padding=(1, 1))
        self.bn1   = nn.BatchNorm2d(64)
        self.conv2 = nn.Conv2d(64,  128,         (3, 3),      padding=(1, 1))
        self.bn2   = nn.BatchNorm2d(128)
        # Collapse the T dimension exactly (kernel width = n_time, no padding)
        self.conv3 = nn.Conv2d(128, out_channels, (3, n_time), padding=(1, 0))
        self.bn3   = nn.BatchNorm2d(out_channels)

        self.head_logits = nn.Linear(out_channels, 4)
        self.head_votes  = nn.Linear(out_channels, 2)

    def forward(self, x: torch.Tensor):
        """x : (N_beams, T, 3)"""
        # Reshape to (1, 3, N_beams, T) for Conv2d
        x = x.permute(2, 0, 1).unsqueeze(0)       # (1, 3, N_beams, T)

        x = F.relu(self.bn1(self.conv1(x)))        # (1, 64, N_beams, T)
        x = F.dropout(x, p=self.dropout_p, training=self.training)
        x = F.relu(self.bn2(self.conv2(x)))        # (1, 128, N_beams, T)
        x = F.dropout(x, p=self.dropout_p, training=self.training)
        x = F.relu(self.bn3(self.conv3(x)))        # (1, out_channels, N_beams, 1)

        x = x.squeeze(-1).squeeze(0)               # (out_channels, N_beams)
        x = x.permute(1, 0)                        # (N_beams, out_channels)
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
            "type":        "SpaceTimeCNNDetector",
            "n_time":      self.n_time,
            "out_channels": self.conv3.out_channels,
            "model":       self.state_dict(),
            "dataset":     dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "SpaceTimeCNNDetector":
        ckpt  = torch.load(path, map_location=map_location)
        model = cls(
            n_time=ckpt.get("n_time", 5),
            out_channels=ckpt.get("out_channels", 128),
        )
        model.load_state_dict(ckpt["model"])
        return model


class FullScanTransformerDetector(nn.Module):
    """
    Full-scan detector: dilated 1D CNN + global beam self-attention + GRU.

    Architecture (Option C from DISCOVERY.md):
      DilatedScanBackbone(3 → 64)  →  BeamSelfAttention(64, 8 heads)  →
      GRU(64 → hidden)  →  detection heads

    The BeamSelfAttention allows every beam to attend to every other beam
    at each timestep (O(N_beams²) per layer), providing content-adaptive,
    unconstrained global context before the temporal aggregation.  This is
    the most expressive of the three full-scan architectures.

    Note: at N_beams=450 with 8 heads the attention tensor is ~3 MB/layer.
    Feasible on GPU; expect ~1.5 s/scan on CPU.
    """

    INPUT_MODE = "full_scan"

    def __init__(self, n_time: int = 5, backbone_channels: int = 64,
                 n_heads: int = 8, hidden: int = 128, dropout: float = 0.1):
        super().__init__()
        self.n_time   = n_time
        self.backbone = DilatedScanBackbone(3, backbone_channels, dropout)
        self.beam_attn = BeamSelfAttention(backbone_channels, n_heads, dropout)
        self.gru      = nn.GRU(backbone_channels, hidden, batch_first=True)
        self.head_logits = nn.Linear(hidden, 4)
        self.head_votes  = nn.Linear(hidden, 2)

    def forward(self, x: torch.Tensor):
        """x : (N_beams, T, 3)"""
        feats  = self.backbone(x)           # (N_beams, T, backbone_channels)
        feats  = self.beam_attn(feats)      # (N_beams, T, backbone_channels)
        _, h   = self.gru(feats)            # h: (1, N_beams, hidden)
        pooled = h.squeeze(0)               # (N_beams, hidden)
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
            "hidden":            self.gru.hidden_size,
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
            hidden=ckpt.get("hidden", 128),
        )
        model.load_state_dict(ckpt["model"])
        return model
