from .detector import Detector
from .drow_detector import DrowDetector
from .algorithmic_detector import AlgorithmicDetector
from .architectures import DrSpaamDetector
from .full_scan import FullScanCNNDetector, SpaceTimeCNNDetector, FullScanTransformerDetector
from .li2former import Li2FormerDetector
from .lfe_detector import LFEPeaksDetector, LFEPPNDetector

# Registry of all trainable/evaluable detectors.
# Keys are the canonical --detector CLI argument values.
# AlgorithmicDetector is included for evaluation but cannot be trained.
# DROW and DR-SPAAM use published weights and are not trained via train.py.
# Li2Former has no published weights; train from scratch or skip.
# LFE-Peaks and LFE-PPN are ONNX-based; evaluation only.
DETECTOR_REGISTRY: dict = {
    "algorithmic":          AlgorithmicDetector,
    "drow":                 DrowDetector,
    "drspaam":              DrSpaamDetector,
    "li2former":            Li2FormerDetector,
    "fullscan_cnn":         FullScanCNNDetector,
    "spacetime_cnn":        SpaceTimeCNNDetector,
    "fullscan_transformer": FullScanTransformerDetector,
    # LFE detectors are ONNX-based and not registered for PyTorch training
    "lfe_peaks":            LFEPeaksDetector,
    "lfe_ppn":              LFEPPNDetector,
}
