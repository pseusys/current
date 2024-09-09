"""
This file is totally based on research papaer about DROW.
Here the link to the paper can be found:
https://arxiv.org/abs/1603.02636
The utility functions present here can be found in paper's GitHub:
https://github.com/VisualComputingInstitute/DROW/blob/master/v2/utils/__init__.py
"""

from json import loads
from pathlib import Path
from typing import Union, List, Dict, Tuple

from numpy import genfromtxt, fromregex, where, array, full, array_equal, vectorize, float32, uint32
from numpy.typing import NDArray

from ..utils.file_utils import DROW_DATA_PATH, DROW_TEST_SET
from ..utils.drow_utils import laser_measures
from ..utils.generic_utils import Logging

_DATASET_PATH = Path(__file__).parent.parent / DROW_DATA_PATH


class DROW_Dataset(Logging):
    TIME_FRAME = 5
    _LOAD_JSON_VECTOR = vectorize(loads, otypes=[object])

    def __init__(self, datapath: Union[Path, str] = _DATASET_PATH, dataset: Union[Path, str] = DROW_TEST_SET, laser_scans: int = laser_measures, time_frame_size: int = TIME_FRAME, verbose: bool = True):
        Logging.__init__(self, verbose)
        self.time_frame = time_frame_size
        dataset_path = Path(datapath) / Path(dataset)
        self.filenames = [f"{f.parent}/{f.stem}" for f in dataset_path.glob("*.csv")]

        self.scan_id: NDArray[uint32]
        self.scan_time: NDArray[float32]
        self.scans: NDArray[float32]

        scan_data = array([self._load_scan(f"{f}.csv", laser_scans) for f in self.filenames], dtype=object)
        self.scan_id, self.scan_time, self.scans = scan_data.transpose()
        self._print(f"Scans from {dataset}/*.csv loaded!")

        self.det_id: NDArray[uint32]
        self.det_wc: NDArray[float32]
        self.det_wa: NDArray[float32]
        self.det_wp: NDArray[float32]

        wc_id, self.det_wc = array([self._load_det(f"{f}.wc") for f in self.filenames], dtype=object).transpose()
        wa_id, self.det_wa = array([self._load_det(f"{f}.wa") for f in self.filenames], dtype=object).transpose()
        wp_id, self.det_wp = array([self._load_det(f"{f}.wp") for f in self.filenames], dtype=object).transpose()
        assert all(array_equal(a, b) and array_equal(a, c) for a, b, c in zip(wc_id, wa_id, wp_id)), "Dataset corrupt!"
        self.det_id = wc_id
        self._print(f"Detections from {dataset}/*.[wc|wa|wp] loaded!")

        self.odoms: NDArray
        self.idet2iscan: List[Dict[int, int]]

        self.odoms = array([self._load_odom(f"{f}.odom2") for f in self.filenames], dtype=object)
        self.idet2iscan = [{i: where(sid == d)[0][0] for i, d in enumerate(did)} for sid, did in zip(self.scan_id, self.det_id)]
        self._print(f"Detections from {dataset}/*.odom2 loaded!")

    @staticmethod
    def _load_scan(fname: Union[Path, str], laser_scans: int) -> NDArray:
        return genfromtxt(fname, delimiter=",", dtype=[("id", uint32), ("time", float32), ("scan", float32, laser_scans)], unpack=True)

    @classmethod
    def _load_det(cls, fname: Union[Path, str]) -> Tuple[NDArray, NDArray]:
        data = fromregex(fname, r"(\d+),(\S+)", dtype=[("id", uint32), ("json", object)])
        return data["id"], cls._LOAD_JSON_VECTOR(data["json"])

    @staticmethod
    def _load_odom(fname: Union[Path, str]) -> NDArray:
        return genfromtxt(fname, delimiter=",", dtype=[("eq", uint32), ("t", float32), ("xya", float32, 3)])

    def get_scan(self, sequence_id: int, scan_id: int, time_window: int) -> Tuple[NDArray, NDArray]:
        start_time = scan_id - time_window + 1
        if start_time < 0:
            scans = full(abs(start_time), self.scans[sequence_id][0]) + self.scans[sequence_id][:scan_id+1]
            odoms = full(abs(start_time), self.odoms[sequence_id][0]) + self.odoms[sequence_id][:scan_id+1]
        else:
            scans = self.scans[sequence_id][start_time:scan_id+1]
            odoms = self.odoms[sequence_id][start_time:scan_id+1]
        return scans, odoms
