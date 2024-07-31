from time import time
from tqdm.auto import trange

from numpy import array

from ..datasets import DROW_Dataset
from ..utils.drow_utils import laser_minimum, laser_increment
from ..cpp_binding import DetectorFactory

from .detector import Detector


class AlgorithmicDetector(Detector):
    _FREQUENCY_INIT=5
    _FREQUENCY_MAX=25
    _UNCERTAINTY_MAX=1
    _UNCERTAINTY_MIN=3
    _UNCERTAINTY_INC=0.05
    _CLUSTER_THRESHOLD=0.1
    _DISTANCE_LEVEL=0.6
    _LEG_SIZE_MIN=0.05
    _LEG_SIZE_MAX=0.25
    _CHEST_SIZE_MIN=0.3
    _CHEST_SIZE_MAX=0.8
    _LEGS_DISTANCE_MIN=0
    _LEGS_DISTANCE_MAX=0.7

    def __init__(self, time_frame_size: int = DROW_Dataset.TIME_FRAME, frequency_init: int = _FREQUENCY_INIT, frequency_max: int = _FREQUENCY_MAX, uncertainty_max: float = _UNCERTAINTY_MAX, uncertainty_min: float = _UNCERTAINTY_MIN, uncertainty_inc: float = _UNCERTAINTY_INC, cluster_threshold: float = _CLUSTER_THRESHOLD, distance_level: float = _DISTANCE_LEVEL, leg_size_min: float = _LEG_SIZE_MIN, leg_size_max: float = _LEG_SIZE_MAX, chest_size_min: float = _CHEST_SIZE_MIN, chest_size_max: float = _CHEST_SIZE_MAX, legs_distance_min: float = _LEGS_DISTANCE_MIN, legs_distance_max: float = _LEGS_DISTANCE_MAX, verbose: bool = True):
        Detector.__init__(self, verbose)
        self.time_frame = time_frame_size
        self._detector = DetectorFactory(laser_minimum, laser_increment, frequency_init, frequency_max, uncertainty_max, uncertainty_min, uncertainty_inc, cluster_threshold, distance_level, leg_size_min, leg_size_max, chest_size_min, chest_size_max, legs_distance_min, legs_distance_max, verbose)

    @classmethod
    def init(cls, verbose: bool = True) -> "Detector":
        return cls(verbose)

    def forward_one(self, scans, odoms):
        return self._detector.forward_one(scans, odoms)

    def forward_all(self, va: DROW_Dataset):
        times = list()
        people = list()
        for iseq in trange(len(va.det_id), desc="Sequences", disable=not self._verbose):
            for idet in trange(len(va.det_id[iseq]), desc="Scans", disable=not self._verbose, leave=False):
                start_time = time()
                iscan = va.idet2iscan[iseq][idet]
                scans, odoms = va.get_scan(iseq, iscan, self.time_frame)
                people += [array(self.forward_one(scans[-1], odoms[-1]["xya"]))]
                times += [time() - start_time]
        self._print(f"Average detection time: {sum(times) / len(times)} seconds")
        return people
