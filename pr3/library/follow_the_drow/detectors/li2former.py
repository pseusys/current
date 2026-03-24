"""
Li2Former: Omni-Dimension Aggregation Transformer for Person Detection in 2D Range Data.

Architecture inspired by:
  Yang et al., IEEE Transactions on Instrumentation and Measurement, 2024
  DOI: 10.1109/TIM.2024.3420353

No published weights are available.  Train from scratch:
  python utils/train.py --detector li2former --dataset frog --epochs 30

Input / output
--------------
  INPUT_MODE  = "cutout"          — fixed-size angular windows around each beam
  N_SAMP      = 64                — points per cutout (vs. 48 for DROW, 56 for DR-SPAAM)
  BEAM_BATCH  = True              — all N beams of one scan stay together
  logits      : (B*N, 1)         — binary person logit (pedestrian vs. background)
  votes       : (B*N, 2)         — (dx, dy) vote offsets in window space
"""

import math
from typing import Tuple

import torch
import torch.nn as nn
import torch.nn.functional as F


def _conv_block(in_ch: int, out_ch: int) -> nn.Sequential:
    """Conv1d(k=3, p=1) → BatchNorm → ReLU."""
    return nn.Sequential(
        nn.Conv1d(in_ch, out_ch, kernel_size=3, padding=1, bias=False),
        nn.BatchNorm1d(out_ch),
        nn.ReLU(inplace=True),
    )


class _ConvBackbone(nn.Module):
    """
    Three-stage 1-D convolutional encoder with progressive spatial downsampling.

    Each stage consists of three conv blocks followed by max-pooling (stages 1-2)
    or adaptive average pooling to a single position (stage 3).

    Input : (B, 1, P)   — single cutout, P range values
    Output: (B, d_model) — fixed-size feature vector per cutout
    """

    def __init__(self, d_model: int = 512):
        super().__init__()
        self.stage1 = nn.Sequential(
            _conv_block(1, 64),
            _conv_block(64, 64),
            _conv_block(64, 128),
            nn.MaxPool1d(2),
        )
        self.stage2 = nn.Sequential(
            _conv_block(128, 128),
            _conv_block(128, 128),
            _conv_block(128, 256),
            nn.MaxPool1d(2),
        )
        self.stage3 = nn.Sequential(
            _conv_block(256, 256),
            _conv_block(256, 256),
            _conv_block(256, d_model),
            nn.AdaptiveAvgPool1d(1),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.stage1(x)
        x = self.stage2(x)
        x = self.stage3(x)
        return x.squeeze(-1)   # (B, d_model)


class Li2FormerDetector(nn.Module):
    """
    Li2Former person detector for 2D LiDAR range data.

    A 1-D convolutional backbone encodes each (beam, timestep) cutout into a
    fixed-size feature vector.  A Transformer encoder then attends over the T
    timesteps for each beam, capturing temporal context.  Classification and
    regression heads produce a binary person confidence and a vote offset.

    Parameters
    ----------
    dropout      : dropout applied to backbone input and pooled features
    num_scans    : temporal window T (number of consecutive scans, default 5)
    d_model      : backbone / Transformer embedding dimension (default 512)
    n_heads      : number of Transformer attention heads (default 8)
    dropout_attn : dropout inside the Transformer (default 0.1)

    Notes
    -----
    - No published weights are available; must be trained from scratch.
    - Binary output (1 logit) is expanded to 4-class format in forward_one()
      by placing sigmoid probability in the wp (pedestrian) slot — identical to
      how DR-SPAAM's pedestrian_only mode is handled.
    """

    INPUT_MODE = "cutout"
    N_SAMP     = 64
    BEAM_BATCH = True

    def __init__(
        self,
        dropout:      float = 0.5,
        num_scans:    int   = 5,
        d_model:      int   = 512,
        n_heads:      int   = 8,
        dropout_attn: float = 0.1,
    ):
        super().__init__()
        self.dropout   = dropout
        self.num_scans = num_scans
        self.d_model   = d_model

        self.backbone = _ConvBackbone(d_model)

        # Fixed sinusoidal positional encoding (not learned)
        self.register_buffer("_pe", self._make_sinusoidal_pe(num_scans, d_model))

        encoder_layer = nn.TransformerEncoderLayer(
            d_model        = d_model,
            nhead          = n_heads,
            dim_feedforward = d_model * 4,
            dropout        = dropout_attn,
            batch_first    = True,
            norm_first     = False,
        )
        self.temporal_encoder = nn.TransformerEncoder(encoder_layer, num_layers=1)

        # Binary classification: person vs. background
        self.cls_head = nn.Linear(d_model, 1)

        # Vote regression: (dx, dy) offsets in window space
        self.reg_head = nn.Sequential(
            nn.Linear(d_model, d_model * 2),
            nn.ReLU(inplace=True),
            nn.Linear(d_model * 2, 2),
            nn.ReLU(inplace=True),
        )

    # ------------------------------------------------------------------
    # Helpers
    # ------------------------------------------------------------------

    @staticmethod
    def _make_sinusoidal_pe(max_len: int, d_model: int) -> torch.Tensor:
        pe       = torch.zeros(max_len, d_model)
        position = torch.arange(max_len, dtype=torch.float).unsqueeze(1)
        div_term = torch.exp(
            torch.arange(0, d_model, 2, dtype=torch.float)
            * (-math.log(10000.0) / d_model)
        )
        pe[:, 0::2] = torch.sin(position * div_term)
        pe[:, 1::2] = torch.cos(position * div_term)
        return pe   # (max_len, d_model)

    # ------------------------------------------------------------------
    # Forward
    # ------------------------------------------------------------------

    def forward(
        self, x: torch.Tensor
    ) -> Tuple[torch.Tensor, torch.Tensor]:
        """
        Parameters
        ----------
        x : (B, N, T, P)  or  (N, T, P) for a single scan

        Returns
        -------
        logits : (B*N, 1)   binary person logit
        votes  : (B*N, 2)   vote offsets (dx, dy)
        """
        single = x.ndim == 3
        if single:
            x = x.unsqueeze(0)   # (1, N, T, P)

        B, N, T, P = x.shape

        # ── Backbone: encode every (scan-step, beam) cutout independently ──
        # Flatten to (B*N*T, 1, P), apply dropout on raw input, run conv stages.
        xr   = x.reshape(B * N * T, 1, P)
        xr   = F.dropout(xr, p=self.dropout, training=self.training)
        feat = self.backbone(xr)              # (B*N*T, d_model)

        # ── Temporal Transformer: attend over T steps per beam ──
        feat = feat.reshape(B * N, T, self.d_model)
        feat = feat + self._pe[:T]            # sinusoidal positional encoding
        feat = self.temporal_encoder(feat)    # (B*N, T, d_model)

        # Mean-pool over the temporal dimension
        feat = feat.mean(dim=1)               # (B*N, d_model)
        feat = F.dropout(feat, p=self.dropout, training=self.training)

        logits = self.cls_head(feat)          # (B*N, 1)
        votes  = self.reg_head(feat)          # (B*N, 2)

        return logits, votes

    # ------------------------------------------------------------------
    # Inference helper (matches DrSpaamDetector interface)
    # ------------------------------------------------------------------

    def forward_one(
        self, xb: torch.Tensor
    ) -> Tuple["np.ndarray", "np.ndarray"]:  # type: ignore[name-defined]
        """
        Single-scan inference with 4-class output expansion.

        Parameters
        ----------
        xb : (N_beams, T, N_SAMP) preprocessed cutout tensor

        Returns
        -------
        confs : (N_beams, 4) numpy — person probability in wp slot (index 3)
        votes : (N_beams, 2) numpy — vote offsets
        """
        import numpy as np
        self.eval()
        with torch.no_grad():
            logits, votes = self(xb)     # (N, 1), (N, 2)

        prob  = torch.sigmoid(logits)
        confs = torch.zeros(logits.shape[0], 4)
        confs[:, 3] = prob[:, 0]         # pedestrian slot
        return confs.cpu().numpy(), votes.cpu().numpy()
