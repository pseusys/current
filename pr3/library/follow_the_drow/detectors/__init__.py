from .detector import Detector
from .drow_detector import DrowDetector
from .algorithmic_detector import AlgorithmicDetector
from .architectures import PersonDetector, DrSpaamDetector, ARCH_REGISTRY
from .full_scan import FullScanCNNDetector, SpaceTimeCNNDetector, FullScanTransformerDetector

# Registry of all six trainable/evaluable detectors.
# Keys are the canonical --detector CLI argument values.
# AlgorithmicDetector is included for evaluation but cannot be trained.
DETECTOR_REGISTRY: dict = {
    "algorithmic":          AlgorithmicDetector,
    "drow":                 DrowDetector,
    "drspaam":              DrSpaamDetector,
    "fullscan_cnn":         FullScanCNNDetector,
    "spacetime_cnn":        SpaceTimeCNNDetector,
    "fullscan_transformer": FullScanTransformerDetector,
}
