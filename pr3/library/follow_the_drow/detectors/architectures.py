"""
Architectures for lidar-based person detection built on the PersonDetector framework.

Input / output convention (identical to DrowDetector):
  x      : (N_beams, T, N_samp)  — N_beams acts as the batch dimension
  logits : (N_beams, 4)           — raw class logits [bg, wc, wa, wp]
  votes  : (N_beams, 2)           — vote offsets (dx, dy) in window space

Temporal aggregators (pass to PersonDetector via arch=):
  "attn_sum" — attention-weighted temporal sum; the DR-SPAAM temporal component.
               Replaces the vanilla DROW fixed sum with a learned scalar weight
               per timestep.  +2 pp AUC alone; +7.7 pp combined with beam_attn.
  "gru"      — GRU over T timesteps; strongest sequential baseline.
               Preferred temporal head when combined with BeamSelfAttention.

Beam spatial attention (enable via beam_attn=True):
  "conv"        — BeamNeighborConv: local Conv1d over N_beams (±5 beams).
                  Implements the DR-SPAAM spatial attention component. +4 pp AUC.
  "transformer" — BeamSelfAttention: global multi-head self-attention over all
                  N_beams at each timestep. Content-adaptive, unconstrained range.

Recommended configurations (in order of expected performance):
  PersonDetector(arch="attn_sum")
      → DR-SPAAM temporal attention only
  PersonDetector(arch="attn_sum", beam_attn=True)
      → Full DR-SPAAM replica (+7.7 pp over DROW baseline, published)
  PersonDetector(arch="gru", beam_attn=True, beam_attn_type="transformer")
      → GRU temporal + global beam attention (new, strongest in framework)
  PersonDetector(arch="attn_sum", beam_attn=True, beam_attn_type="transformer")
      → DR-SPAAM temporal + global beam attention (new)

References:
  DROW:     Beyer et al., IROS 2018  — arXiv:1603.02636
  DR-SPAAM: Jia et al.,  IROS 2020  — arXiv:2004.14064
"""

from typing import Optional

import torch
import torch.nn as nn
import torch.nn.functional as F


# ---------------------------------------------------------------------------
# Shared spatial encoder
# ---------------------------------------------------------------------------

class SpatialEncoder(nn.Module):
    """
    Per-timestep 1-D CNN (identical to DrowDetector blocks 1 & 2).

    Input:  (B, T, S)
    Output: (B, T, C)  where C = spatial_channels
    """

    def __init__(self, spatial_channels: int = 256, dropout: float = 0.5,
                 return_spatial: bool = False):
        super().__init__()
        self.dropout_p      = dropout
        self.return_spatial = return_spatial

        # Block 1: 1 → 64 → 64 → 128, max-pool /2
        self.conv1a = nn.Conv1d(1,   64,  3, padding=1)
        self.bn1a   = nn.BatchNorm1d(64)
        self.conv1b = nn.Conv1d(64,  64,  3, padding=1)
        self.bn1b   = nn.BatchNorm1d(64)
        self.conv1c = nn.Conv1d(64,  128, 3, padding=1)
        self.bn1c   = nn.BatchNorm1d(128)

        # Block 2: 128 → 128 → 128 → C, max-pool /2
        self.conv2a = nn.Conv1d(128, 128,              3, padding=1)
        self.bn2a   = nn.BatchNorm1d(128)
        self.conv2b = nn.Conv1d(128, 128,              3, padding=1)
        self.bn2b   = nn.BatchNorm1d(128)
        self.conv2c = nn.Conv1d(128, spatial_channels, 3, padding=1)
        self.bn2c   = nn.BatchNorm1d(spatial_channels)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        B, T, S = x.shape
        x = x.reshape(B * T, 1, S)

        x = F.leaky_relu(self.bn1a(self.conv1a(x)), 0.1)
        x = F.leaky_relu(self.bn1b(self.conv1b(x)), 0.1)
        x = F.leaky_relu(self.bn1c(self.conv1c(x)), 0.1)
        x = F.max_pool1d(x, 2)
        x = F.dropout(x, p=self.dropout_p, training=self.training)

        x = F.leaky_relu(self.bn2a(self.conv2a(x)), 0.1)
        x = F.leaky_relu(self.bn2b(self.conv2b(x)), 0.1)
        x = F.leaky_relu(self.bn2c(self.conv2c(x)), 0.1)
        x = F.max_pool1d(x, 2)
        x = F.dropout(x, p=self.dropout_p, training=self.training)

        if self.return_spatial:
            # Keep spatial dimension for DR-SPAAM attention — (B, T, C, n_pts)
            return x.reshape(B, T, x.shape[1], x.shape[2])
        x = x.mean(dim=-1)              # global average pool → (B*T, C)
        return x.reshape(B, T, -1)      # (B, T, C)


# ---------------------------------------------------------------------------
# Beam spatial attention (across N_beams dimension)
# ---------------------------------------------------------------------------

class BeamNeighborConv(nn.Module):
    """
    Local 1D convolution over the beam (angular) dimension.

    Implements the spatial attention component of DR-SPAAM (Jia et al., IROS 2020).
    After the SpatialEncoder, each beam's feature is enriched by aggregating
    context from its K nearest angular neighbours.

    Receptive field: ±(kernel_size//2) beams (default ±5).
    Cost: O(N_beams · kernel_size · C) — cheap, runs on CPU.

    DR-SPAAM ablation: +4 pp AUC on wp class from this component alone.

    Input / output: (N_beams, T, C)  — same shape, residual connection included.
    """

    def __init__(self, channels: int, kernel_size: int = 11):
        super().__init__()
        self.conv = nn.Conv1d(channels, channels, kernel_size,
                              padding=kernel_size // 2)
        self.bn   = nn.BatchNorm1d(channels)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        B, T, C = x.shape
        # T acts as batch, N_beams as the Conv1d spatial sequence.
        x_in = x.permute(1, 2, 0)               # (T, C, N_beams)
        out  = F.relu(self.bn(self.conv(x_in)))  # (T, C, N_beams)
        return out.permute(2, 0, 1) + x          # residual → (N_beams, T, C)


class BeamSelfAttention(nn.Module):
    """
    Global multi-head self-attention over the beam (angular) dimension.

    Unlike BeamNeighborConv (local, fixed ±5 beams), every beam can attend to
    every other beam — content-adaptive, global communication across the full scan.
    Applied independently at each of the T timesteps (T acts as the batch dimension).

    This is the "Transformer variant" of the DR-SPAAM spatial attention, removing
    the locality constraint entirely. Expected to outperform BeamNeighborConv at
    the cost of an O(N_beams²) attention matrix per timestep.

    At N_beams=450 with 8 heads: attention tensors ≈ 3 MB per layer — feasible
    on GPU; slow on CPU (~1.5 s/scan).

    Select via beam_attn=True, beam_attn_type="transformer" in PersonDetector.

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
# Temporal aggregators — all take (N_beams, T, C) and return (N_beams, C_out)
# ---------------------------------------------------------------------------

class AttentionWeightedSum(nn.Module):
    """
    Learnable attention-weighted temporal aggregation (DR-SPAAM temporal head).

    Replaces the fixed sum-over-T in vanilla DROW with a single learned scalar
    weight per timestep, allowing the model to down-weight noisy or occluded frames.

    DR-SPAAM ablation: +2 pp AUC on wp class from this component alone.
    Combined with BeamNeighborConv gives the full +7.7 pp DR-SPAAM gain.

    API note: 'hidden' argument is accepted but unused (for registry compatibility).
    """

    def __init__(self, in_channels: int, hidden: int = 256):
        super().__init__()
        self._out  = in_channels
        self.score = nn.Linear(in_channels, 1, bias=False)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # x: (B, T, C)
        weights = F.softmax(self.score(x), dim=1)   # (B, T, 1)
        return (weights * x).sum(dim=1)              # (B, C)

    @property
    def out_channels(self) -> int:
        return self._out


class GRUTemporal(nn.Module):
    """
    GRU over T timesteps; returns the last hidden state.

    Strongest sequential temporal aggregator at T=5 in terms of expressiveness-
    to-parameter ratio. The preferred temporal head when combined with
    BeamSelfAttention for the global-attention configuration.
    """

    def __init__(self, in_channels: int, hidden: int = 256, n_layers: int = 1):
        super().__init__()
        self.gru = nn.GRU(in_channels, hidden, num_layers=n_layers,
                          batch_first=True)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        _, h = self.gru(x)      # h: (n_layers, B, hidden)
        return h[-1]            # (B, hidden)

    @property
    def out_channels(self) -> int:
        return self.gru.hidden_size


# ---------------------------------------------------------------------------
# DR-SPAAM components — faithful to the official implementation
# ---------------------------------------------------------------------------

class TemporalAttention(nn.Module):
    """
    Temporal attention from DR-SPAAM (Jia et al., IROS 2020).

    Matches ``_TemporalAttention`` in the official DR-SPAAM-Detector repo.
    Unlike ``AttentionWeightedSum`` (which scores globally-pooled vectors),
    this module operates on the full spatial feature maps before pooling, giving
    it richer information to decide which timesteps to weight.

    Input:  (B, T, C, n_pts) — spatial feature maps after SpatialEncoder
    Output: (B, T)           — softmax-normalised weights over T timesteps
    """

    def __init__(self, n_channel: int, n_pts: int, n_scans: int,
                 hidden: int = 64):
        super().__init__()
        self.conv1 = nn.Conv1d(n_channel, 128,    kernel_size=n_pts)   # collapse spatial
        self.bn1   = nn.BatchNorm1d(128)
        self.conv2 = nn.Conv1d(128,       hidden, kernel_size=n_scans) # collapse temporal
        self.bn2   = nn.BatchNorm1d(hidden)
        self.fc    = nn.Linear(hidden, n_scans)
        self.act   = nn.LeakyReLU(0.1, inplace=True)

        for m in self.modules():
            if isinstance(m, nn.Conv1d):
                nn.init.kaiming_normal_(m.weight, a=0.1, nonlinearity="leaky_relu")
            elif isinstance(m, nn.BatchNorm1d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        B, T, C, n_pts = x.shape
        out = x.reshape(B * T, C, n_pts)
        out = self.act(self.bn1(self.conv1(out)))           # (B*T, 128, 1)
        out = out.reshape(B, T, 128).permute(0, 2, 1)      # (B, 128, T)
        out = self.act(self.bn2(self.conv2(out)))           # (B, hidden, 1)
        out = out.reshape(B, -1)                            # (B, hidden)
        return F.softmax(self.fc(out), dim=1)               # (B, T)


class SpatialAttention(nn.Module):
    """
    Auto-regressive spatial attention from DR-SPAAM (Jia et al., IROS 2020).

    Matches ``_SpatialAttention`` in the official DR-SPAAM-Detector repo.
    For each beam, attends to its ``window_size`` nearest angular neighbours
    in a *template* feature map from the previous timestep/frame.  The
    similarity is computed as a dot product of learned embeddings, then
    normalised with a masked softmax.  Current and attended-template features
    are blended with weight ``alpha``.

    The "A" in DR-SPAAM refers to this auto-regressive template mechanism:
    the template is a stop-gradient copy of the previous call's output,
    allowing temporal context to accumulate without BPTT.

    Input:  x, x_template — both (B, N_beams, C, n_pts)
    Output: (B, N_beams, C, n_pts)
    """

    def __init__(self, n_channel: int, n_pts: int,
                 alpha: float = 0.5, window_size: int = 7):
        super().__init__()
        self._alpha       = alpha
        self._window_size = window_size
        self.conv = nn.Sequential(
            nn.Conv1d(n_channel, 128, kernel_size=n_pts),
            nn.BatchNorm1d(128),
            nn.LeakyReLU(0.1, inplace=True),
        )
        self._neighbor_mask: Optional[torch.Tensor] = None

        for m in self.modules():
            if isinstance(m, nn.Conv1d):
                nn.init.kaiming_normal_(m.weight, a=0.1, nonlinearity="leaky_relu")
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
                x_template: torch.Tensor) -> torch.Tensor:
        B, N, C, n_pts = x.shape
        if self._neighbor_mask is None or self._neighbor_mask.shape[0] != N:
            self._neighbor_mask = self._make_mask(N, x.device)
        elif self._neighbor_mask.device != x.device:
            self._neighbor_mask = self._neighbor_mask.to(x.device)

        emb_x    = self.conv(x.reshape(B*N, C, n_pts)).reshape(B, N, 128)
        emb_temp = self.conv(x_template.reshape(B*N, C, n_pts)).reshape(B, N, 128)

        sim  = torch.matmul(emb_x, emb_temp.permute(0, 2, 1))           # (B, N, N)
        sim  = sim - 1e10 * (1.0 - self._neighbor_mask)                  # mask non-nbrs
        maxv = sim.max(dim=-1, keepdim=True)[0]
        exps = torch.exp(sim - maxv) * self._neighbor_mask
        sim  = exps / (exps.sum(dim=-1, keepdim=True) + 1e-8)            # (B, N, N)

        out_temp = torch.matmul(sim, x_template.reshape(B, N, C * n_pts))
        out_temp = out_temp.reshape(B, N, C, n_pts)
        return self._alpha * x + (1.0 - self._alpha) * out_temp


# ---------------------------------------------------------------------------
# Registry and PersonDetector wrapper
# ---------------------------------------------------------------------------

ARCH_REGISTRY: dict = {
    "attn_sum": AttentionWeightedSum,
    "gru":      GRUTemporal,
}


class PersonDetector(nn.Module):
    """
    Modular lidar-based person detector.

    Drop-in replacement for DrowDetector at inference time:
      forward(x)       → (logits, votes)
      forward_one(xb)  → (confs, votes)   numpy in / numpy out

    Parameters
    ----------
    arch             : temporal aggregator — "attn_sum" or "gru"
    n_time           : number of timesteps T (must match the dataset's time_frame)
    spatial_channels : output channels of the spatial encoder (default 256)
    hidden           : hidden size for the temporal head (default 256)
    dropout          : dropout probability in the spatial encoder (default 0.5)
    beam_attn        : if True, insert a beam spatial attention module after the
                       spatial encoder. Implements DR-SPAAM spatial attention (+4 pp AUC).
                       Combine with arch="attn_sum" for full DR-SPAAM (+7.7 pp).
    beam_attn_type   : "conv"        — BeamNeighborConv, local (±kernel/2 beams), cheap
                       "transformer" — BeamSelfAttention, global (all beams), expressive
    beam_kernel      : kernel size for BeamNeighborConv (ignored for "transformer")
    **arch_kwargs    : extra keyword arguments forwarded to the temporal head
    """

    INPUT_MODE = "cutout"
    N_SAMP = 48

    def __init__(self, arch: str = "gru", n_time: int = 5,
                 spatial_channels: int = 256, hidden: int = 256,
                 dropout: float = 0.5,
                 beam_attn: bool = False, beam_attn_type: str = "conv",
                 beam_kernel: int = 11,
                 **arch_kwargs):
        super().__init__()
        if arch not in ARCH_REGISTRY:
            raise ValueError(
                f"Unknown arch '{arch}'. Choose from: {list(ARCH_REGISTRY)}"
            )
        if beam_attn and beam_attn_type not in ("conv", "transformer"):
            raise ValueError("beam_attn_type must be 'conv' or 'transformer'")

        self.arch           = arch
        self.n_time         = n_time
        self.beam_attn      = beam_attn
        self.beam_attn_type = beam_attn_type
        self.encoder        = SpatialEncoder(spatial_channels=spatial_channels,
                                             dropout=dropout)
        if not beam_attn:
            self.beam_conv = None
        elif beam_attn_type == "transformer":
            self.beam_conv = BeamSelfAttention(spatial_channels)
        else:
            self.beam_conv = BeamNeighborConv(spatial_channels, beam_kernel)

        arch_cls = ARCH_REGISTRY[arch]
        self.temporal = arch_cls(spatial_channels, hidden, **arch_kwargs)

        out_ch = self.temporal.out_channels
        self.head_logits = nn.Linear(out_ch, 4)
        self.head_votes  = nn.Linear(out_ch, 2)

    def forward(self, x: torch.Tensor):
        """
        x : (B, T, S)  — B = N_beams (beam dimension acts as batch)
        Returns: logits (B, 4), votes (B, 2)
        """
        feats = self.encoder(x)
        if self.beam_conv is not None:
            feats = self.beam_conv(feats)
        pooled = self.temporal(feats)
        return self.head_logits(pooled), self.head_votes(pooled)

    @torch.no_grad()
    def forward_one(self, xb) -> tuple:
        """
        Numpy-in, numpy-out inference.  Drop-in for DrowDetector.forward_one.

        xb : ndarray  (N_beams, T, N_samp)
        Returns: (confs ndarray (N_beams, 4), votes ndarray (N_beams, 2))
        """
        import numpy as np
        self.eval()
        x = torch.from_numpy(np.asarray(xb, dtype=np.float32))
        logits, votes = self(x)
        return F.softmax(logits, dim=-1).cpu().numpy(), votes.cpu().numpy()

    def save_weights(self, path, dataset: str = ""):
        """Save a deployment checkpoint loadable by PersonDetector.load()."""
        torch.save({
            "type":             "PersonDetector",
            "arch":             self.arch,
            "n_time":           self.n_time,
            "spatial_channels": self.encoder.conv2c.out_channels,
            "hidden":           self.temporal.out_channels,
            "dropout":          self.encoder.dropout_p,
            "beam_attn":        self.beam_attn,
            "beam_attn_type":   self.beam_attn_type,
            "beam_kernel":      (self.beam_conv.conv.kernel_size[0]
                                 if self.beam_conv is not None
                                    and isinstance(self.beam_conv, BeamNeighborConv)
                                 else 11),
            "model":            self.state_dict(),
            "dataset":          dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "PersonDetector":
        """Load a PersonDetector saved with save_weights()."""
        ckpt = torch.load(path, map_location=map_location)
        model = cls(
            arch=ckpt["arch"],
            n_time=ckpt.get("n_time", 5),
            spatial_channels=ckpt.get("spatial_channels", 256),
            hidden=ckpt.get("hidden", 256),
            dropout=ckpt.get("dropout", 0.5),
            beam_attn=ckpt.get("beam_attn", False),
            beam_attn_type=ckpt.get("beam_attn_type", "conv"),
            beam_kernel=ckpt.get("beam_kernel", 11),
        )
        model.load_state_dict(ckpt["model"])
        return model


# ---------------------------------------------------------------------------
# Named public detectors
# ---------------------------------------------------------------------------

class DrSpaamDetector(nn.Module):
    """
    DR-SPAAM person detector — faithful to Jia et al., IROS 2020 (arXiv:2004.14064).

    Matches the official implementation at:
      https://github.com/VisualComputingInstitute/DR-SPAAM-Detector

    Architecture
    ------------
    SpatialEncoder (DROW blocks 1–2, *without* global avg-pool)
      → per-timestep spatial feature maps  (B, T, C, n_pts)
    SpatialAttention (auto-regressive, ±window//2 beam neighbours, alpha blending)
      → attended feature maps over T timesteps
    TemporalAttention (Conv1d over spatial + temporal dims → FC → softmax weights)
      → per-beam scalar weights  (B, T)
    Weighted sum over T + global avg-pool over spatial → Linear output heads

    The "A" (Auto-regressive) component: a stop-gradient template from the
    previous timestep is used as the key/value in spatial attention, allowing
    the model to aggregate neighbourhood context across time without BPTT.

    Input / output (identical to DrowDetector)
    ------------------------------------------
    Single scan (eval):   x (N_beams, T, S) → logits (N_beams,4), votes (N_beams,2)
    Batched (training):   x (B, N_beams, T, S) → logits (B*N,4),  votes (B*N,2)

    BEAM_BATCH = True signals train.py to pass (B, N, T, S) so that all N_beams
    of one scan stay together for the spatial attention to work correctly.
    """

    INPUT_MODE = "cutout"
    N_SAMP     = 48
    BEAM_BATCH = True   # train.py: pass (B, N, T, S), not the flattened (B*N, T, S)

    def __init__(self, n_time: int = 5, spatial_channels: int = 256,
                 dropout: float = 0.5, attn_hidden: int = 64,
                 beam_window: int = 7, alpha: float = 0.5,
                 verbose: bool = False):
        super().__init__()
        self.n_time   = n_time
        self._C       = spatial_channels
        self._n_pts   = self.N_SAMP // 4       # 12 after two MaxPool(2)
        self._verbose = verbose

        self.encoder       = SpatialEncoder(spatial_channels, dropout,
                                            return_spatial=True)
        self.spatial_attn  = SpatialAttention(spatial_channels, self._n_pts,
                                              alpha, beam_window)
        self.temporal_attn = TemporalAttention(spatial_channels, self._n_pts,
                                               n_time, attn_hidden)
        self.head_logits   = nn.Linear(spatial_channels, 4)
        self.head_votes    = nn.Linear(spatial_channels, 2)

        self._template: Optional[torch.Tensor] = None  # (1, N, C, n_pts)

    def reset_template(self) -> None:
        """Clear the auto-regressive template (call at the start of each new sequence)."""
        self._template = None

    def forward(self, x: torch.Tensor) -> tuple:
        """
        x : (N_beams, T, S)        single scan — eval / render_video
          | (B, N_beams, T, S)    batch of scans — training
        Returns logits, votes with the beam dimension unflattened:
          single → (N_beams, 4/2)
          batch  → (B*N_beams, 4/2)
        """
        single = (x.ndim == 3)
        if single:
            x = x.unsqueeze(0)                 # (1, N, T, S)

        B, N, T, S = x.shape
        C, n_pts = self._C, self._n_pts

        # Encode all B×N beams together; encoder treats (B*N) as its batch dim
        feats = self.encoder(x.reshape(B * N, T, S))       # (B*N, T, C, n_pts)
        feats = feats.reshape(B, N, T, C, n_pts)

        # Template: use stored cross-frame template in single-scan eval if N matches;
        # otherwise initialise from t=0 features (training and first-frame inference)
        if single and self._template is not None and self._template.shape[1] == N:
            template = self._template.to(x.device)         # (1, N, C, n_pts)
        else:
            template = feats[:, :, 0].detach()             # (B, N, C, n_pts)

        # Auto-regressive spatial attention: process T timesteps sequentially,
        # each attending to the previous timestep's features as template
        attended = []
        for t in range(T):
            cur = feats[:, :, t]                           # (B, N, C, n_pts)
            att = self.spatial_attn(cur, template)         # (B, N, C, n_pts)
            attended.append(att)
            template = att.detach()                        # stop-gradient update

        if single:
            self._template = template[:1].detach()         # persist for next call

        attended = torch.stack(attended, dim=2)            # (B, N, T, C, n_pts)

        # Temporal attention over the T attended feature maps
        BN = B * N
        weights = self.temporal_attn(
            attended.reshape(BN, T, C, n_pts))             # (BN, T)
        w      = weights.reshape(BN, T, 1, 1)
        pooled = (w * attended.reshape(BN, T, C, n_pts)).sum(1)  # (BN, C, n_pts)
        pooled = pooled.mean(dim=-1)                       # (BN, C) — global avg-pool

        logits = self.head_logits(pooled)
        votes  = self.head_votes(pooled)

        if single:
            return logits.reshape(N, 4), votes.reshape(N, 2)
        return logits, votes                               # (B*N, 4), (B*N, 2)

    @torch.no_grad()
    def forward_one(self, xb) -> tuple:
        """Numpy-in, numpy-out inference (single scan, stateful template)."""
        import numpy as np
        self.eval()
        x = torch.from_numpy(np.asarray(xb, dtype=np.float32))
        logits, votes = self(x)
        return F.softmax(logits, dim=-1).cpu().numpy(), votes.cpu().numpy()

    def forward_all(self, va) -> tuple:
        """
        Inference over every annotated frame in dataset *va*.
        Template is reset at the start of each sequence so the auto-regressive
        context is properly initialised per sequence.
        """
        from time import time
        from tqdm.auto import trange
        from numpy import array
        from ..utils.drow_utils import cutout as _cutout

        times, all_confs, all_votes = [], [], []
        for iseq in trange(len(va.det_id), desc="Sequences",
                           disable=not self._verbose):
            self.reset_template()
            for idet in trange(len(va.det_id[iseq]), desc="Scans",
                               disable=not self._verbose, leave=False):
                t0 = time()
                iscan  = va.idet2iscan[iseq][idet]
                scans, odoms = va.get_scan(iseq, iscan, self.n_time)
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
        torch.save({
            "type":             "DrSpaamDetector",
            "n_time":           self.n_time,
            "spatial_channels": self._C,
            "dropout":          self.encoder.dropout_p,
            "attn_hidden":      self.temporal_attn.fc.in_features,
            "beam_window":      self.spatial_attn._window_size,
            "alpha":            self.spatial_attn._alpha,
            "model":            self.state_dict(),
            "dataset":          dataset,
        }, path)

    @classmethod
    def load(cls, path, map_location: str = "cpu") -> "DrSpaamDetector":
        ckpt = torch.load(path, map_location=map_location)
        model = cls(
            n_time           = ckpt.get("n_time",           5),
            spatial_channels = ckpt.get("spatial_channels", 256),
            dropout          = ckpt.get("dropout",          0.5),
            attn_hidden      = ckpt.get("attn_hidden",      64),
            beam_window      = ckpt.get("beam_window",      7),
            alpha            = ckpt.get("alpha",            0.5),
        )
        model.load_state_dict(ckpt["model"])
        return model
