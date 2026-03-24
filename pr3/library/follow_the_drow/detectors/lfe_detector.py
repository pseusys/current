"""
LFE-Peaks and LFE-PPN detectors from the FROG benchmark (ONNX inference).

Both detectors are inference-only wrappers around published ONNX weights from:
  Amodeo, Pérez-Higueras, Merino, Caballero
  "FROG: A New People Detection Dataset for Knee-High 2D Range Finders"
  Frontiers in Robotics and AI, 2025 — arXiv:2306.08531

Weights are downloaded automatically during ``pip install ./library``.
Download URLs:
  LFE-Peaks : https://robotics.upo.es/~famozur/onnx/LFE-Peaks.onnx
  LFE-PPN   : https://robotics.upo.es/~famozur/onnx/LFE-PPN.onnx

Architecture
------------
Both detectors share an LFE backbone: a 1-D U-Net FCN operating directly on
the raw laser scan vector (720 beams for FROG, 450 for DROW).  They differ in
the output head:

  LFE-Peaks  : per-beam sigmoid probability → scipy find_peaks post-processing
  LFE-PPN    : 1-D region proposal network on sector-anchor grid → NMS decoding

Key constraints
---------------
- Single scan only (no temporal history).
- Trained on 720-beam FROG scans; inference on 450-beam DROW scans requires
  zero-padding to 720 beams (accuracy may degrade).
- Both models return detections as class-agnostic (pedestrian) confidence only.
"""

from pathlib import Path
from typing import List, Tuple

import numpy as np

from ..utils.file_utils import LFE_PEAKS_WEIGHTS_PATH, LFE_PPN_WEIGHTS_PATH

_LFE_PEAKS_PATH = Path(__file__).parent.parent / LFE_PEAKS_WEIGHTS_PATH
_LFE_PPN_PATH   = Path(__file__).parent.parent / LFE_PPN_WEIGHTS_PATH

# Physical constants matching the FROG training configuration
_SCAN_NEAR       = 0.2    # metres — minimum range clipped
_SCAN_FAR        = 10.0   # metres — maximum range clipped
_PERSON_RADIUS   = 0.4    # metres — assumed person half-width
_TRAINED_N_BEAMS = 720    # beam count the ONNX models were trained on


def _normalize_scan(scan: np.ndarray) -> np.ndarray:
    """
    Normalise raw range values to [0, 1].

    Closer obstacles → higher value (1.0 at SCAN_NEAR, 0.0 at SCAN_FAR).
    Missing / max-range readings are mapped to 0.0.
    """
    clipped = np.clip(scan, _SCAN_NEAR, _SCAN_FAR)
    return (1.0 - clipped / _SCAN_FAR).astype(np.float32)


def _pad_to_trained(scan: np.ndarray) -> np.ndarray:
    """Zero-pad a shorter scan to _TRAINED_N_BEAMS if necessary."""
    n = len(scan)
    if n == _TRAINED_N_BEAMS:
        return scan
    if n > _TRAINED_N_BEAMS:
        return scan[:_TRAINED_N_BEAMS]
    padded = np.zeros(_TRAINED_N_BEAMS, dtype=scan.dtype)
    padded[:n] = scan
    return padded


def _load_onnx(path: Path):
    """Load an ONNX InferenceSession (CPU provider)."""
    try:
        import onnxruntime as ort
    except ImportError as e:
        raise ImportError(
            "onnxruntime is required for LFE detectors. "
            "Install it with: pip install onnxruntime"
        ) from e

    if not path.exists():
        raise FileNotFoundError(
            f"LFE ONNX weights not found at {path}.\n"
            "Re-install the library to re-download them:\n"
            "  pip install ./library"
        )
    return ort.InferenceSession(
        str(path), providers=["CPUExecutionProvider"]
    )


# ---------------------------------------------------------------------------
# LFE-Peaks
# ---------------------------------------------------------------------------

class LFEPeaksDetector:
    """
    LFE-Peaks: 1-D U-Net FCN + scipy find_peaks post-processing.

    Returns class-agnostic person detections from a single raw scan.

    Parameters
    ----------
    onnx_path : path to the LFE-Peaks ONNX file (default: bundled weights)
    peak_height      : minimum peak height threshold (default 0.01)
    peak_prominence  : minimum prominence for find_peaks (default 0.1)
    peak_width       : minimum width in samples (default 1)
    """

    DEFAULT_WEIGHTS = _LFE_PEAKS_PATH

    def __init__(
        self,
        onnx_path:       Path  = _LFE_PEAKS_PATH,
        peak_height:     float = 0.01,
        peak_prominence: float = 0.1,
        peak_width:      int   = 1,
    ):
        self._session        = _load_onnx(onnx_path)
        self._input_name     = self._session.get_inputs()[0].name
        self._output_name    = self._session.get_outputs()[0].name
        self._peak_height    = peak_height
        self._peak_prominence = peak_prominence
        self._peak_width     = peak_width

    def detect(
        self,
        scan:   np.ndarray,
        angles: np.ndarray,
    ) -> List[Tuple[float, float, float]]:
        """
        Detect people in a single raw scan.

        Parameters
        ----------
        scan   : (N,) float32  raw range measurements in metres
        angles : (N,) float64  beam angles in radians

        Returns
        -------
        list of (confidence, x, y) in robot-frame Cartesian coordinates
        """
        from scipy.signal import find_peaks

        n_orig = len(scan)
        scan_p = _pad_to_trained(scan)
        norm   = _normalize_scan(scan_p)

        # ONNX models expect (batch, steps, channels) — Keras Conv1D layout
        inp = norm.reshape(1, _TRAINED_N_BEAMS, 1)
        out = self._session.run(
            [self._output_name], {self._input_name: inp}
        )[0]                                        # (1, N, 1) or (1, N)
        prob = out.squeeze()[:n_orig]               # (n_orig,)

        peaks, _ = find_peaks(
            prob,
            height     = self._peak_height,
            prominence = self._peak_prominence,
            width      = self._peak_width,
            rel_height = 0.5,
        )

        detections = []
        for pk in peaks:
            score = float(prob[pk])
            r     = float(scan[pk])
            phi   = float(angles[pk])
            x     = r * -np.sin(phi)
            y     = r *  np.cos(phi)
            detections.append((score, x, y))

        return detections


# ---------------------------------------------------------------------------
# LFE-PPN
# ---------------------------------------------------------------------------

class LFEPPNDetector:
    """
    LFE-PPN: LFE backbone + 1-D polar region proposal network.

    Divides the FoV into N/6 angular sectors with M range anchors each,
    predicts objectness + distance/angle offsets per anchor, then decodes and
    applies NMS.

    Parameters
    ----------
    onnx_path     : path to the LFE-PPN ONNX file (default: bundled weights)
    score_thresh  : minimum objectness score to keep a proposal (default 0.3)
    nms_radius    : minimum distance (m) between surviving detections (default 0.8)
    """

    DEFAULT_WEIGHTS = _LFE_PPN_PATH

    # Anchor parameters matching the FROG training configuration
    _SECTOR_STRIDE = 6     # one anchor sector every 6 beams
    _N_ANCHORS     = 31    # range anchors per sector: (FAR-NEAR)/(0.8*RADIUS)
    _DEPTH_SPACING = (_SCAN_FAR - _SCAN_NEAR) / (_N_ANCHORS - 1)

    def __init__(
        self,
        onnx_path:    Path  = _LFE_PPN_PATH,
        score_thresh: float = 0.3,
        nms_radius:   float = 0.8,
    ):
        self._session      = _load_onnx(onnx_path)
        self._input_name   = self._session.get_inputs()[0].name
        self._output_name  = self._session.get_outputs()[0].name
        self._score_thresh = score_thresh
        self._nms_radius   = nms_radius

        # Pre-compute anchor depths
        self._anchor_depths = np.linspace(
            _SCAN_NEAR, _SCAN_FAR, self._N_ANCHORS, dtype=np.float32
        )

    @staticmethod
    def _sigmoid(x: np.ndarray) -> np.ndarray:
        return 1.0 / (1.0 + np.exp(-np.clip(x, -30, 30)))

    @staticmethod
    def _nms(
        detections: List[Tuple[float, float, float]],
        radius:     float,
    ) -> List[Tuple[float, float, float]]:
        """Greedy distance-based non-maximum suppression."""
        if not detections:
            return []
        dets = sorted(detections, key=lambda d: -d[0])
        keep = []
        for det in dets:
            _, x, y = det
            if all(
                np.sqrt((x - kx) ** 2 + (y - ky) ** 2) > radius
                for _, kx, ky in keep
            ):
                keep.append(det)
        return keep

    def detect(
        self,
        scan:   np.ndarray,
        angles: np.ndarray,
    ) -> List[Tuple[float, float, float]]:
        """
        Detect people in a single raw scan via anchor grid decoding.

        Parameters
        ----------
        scan   : (N,) float32  raw range measurements in metres
        angles : (N,) float64  beam angles in radians

        Returns
        -------
        list of (confidence, x, y) in robot-frame Cartesian coordinates
        """
        n_orig = len(scan)
        scan_p = _pad_to_trained(scan)
        norm   = _normalize_scan(scan_p)

        inp = norm.reshape(1, _TRAINED_N_BEAMS, 1)
        out = self._session.run(
            [self._output_name], {self._input_name: inp}
        )[0]                        # (1, N_sectors, M, 3) or (N_sectors, M, 3)

        while out.ndim > 3:
            out = out.squeeze(0)    # (N_sectors, M, 3)

        n_sectors = out.shape[0]

        # Sector centre angles: take the middle beam of each sector
        sector_beam_indices = np.arange(n_sectors) * self._SECTOR_STRIDE + self._SECTOR_STRIDE // 2
        sector_beam_indices = np.clip(sector_beam_indices, 0, n_orig - 1)
        # Use angles from the original (un-padded) scan where available
        sector_angles = angles[sector_beam_indices]

        objectness = self._sigmoid(out[:, :, 0])  # (N_sectors, M)
        d_offset   = out[:, :, 1]                  # distance offset
        l_offset   = out[:, :, 2]                  # arc offset (normalised)

        # Decode all anchors above threshold
        detections = []
        for s in range(n_sectors):
            for m in range(self._N_ANCHORS):
                score = float(objectness[s, m])
                if score < self._score_thresh:
                    continue
                anchor_d   = float(self._anchor_depths[m])
                final_d    = anchor_d + float(d_offset[s, m])
                # Arc offset normalised by depth spacing; convert to angle offset
                phi_offset = (float(l_offset[s, m]) * self._DEPTH_SPACING
                              / max(anchor_d, 0.1))
                final_phi  = float(sector_angles[s]) + phi_offset

                x = final_d * -np.sin(final_phi)
                y = final_d *  np.cos(final_phi)
                detections.append((score, x, y))

        return self._nms(detections, self._nms_radius)
