from collections import deque
from typing import Tuple, Optional

from numpy import array
from numpy.typing import NDArray

from .drow_dataset import DROW_Dataset
from ..utils.generic_utils import Logging


class LiveDataset(Logging):
    def __init__(self, time_frame_size: int = DROW_Dataset.TIME_FRAME, verbose: bool = True):
        Logging.__init__(self, verbose)
        self.time_frame = time_frame_size
        self.bottom_scans = deque(list())
        self.top_scans = deque(list())

    def push_measure(self, bottom_scans: NDArray, top_scans: NDArray, odometry: NDArray):
        if len(bottom_scans) > 0:
            if len(self.bottom_scans) == 0:
                self.bottom_scans = deque([(bottom_scans, odometry)] * self.time_frame, self.time_frame)
            else:
                self.bottom_scans.append((bottom_scans, odometry))
        if len(top_scans) > 0:
            if len(self.top_scans) == 0:
                self.top_scans = deque([(top_scans, odometry)] * self.time_frame, self.time_frame)
            else:
                self.top_scans.append((top_scans, odometry))

    def get_bottom_scan(self, time_window: Optional[int] = None) -> Tuple[NDArray, NDArray]:
        time_window = self.time_frame if time_window is None else time_window
        scans, odoms = tuple(zip(*list(self.bottom_scans)[-time_window:]))
        return array(scans), array(odoms)
    
    def get_top_scan(self, time_window: Optional[int] = None) -> Tuple[NDArray, NDArray]:
        time_window = self.time_frame if time_window is None else time_window
        scans, odoms = tuple(zip(*list(self.top_scans)[-time_window:]))
        return array(scans), array(odoms)
