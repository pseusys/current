"""
JRDB dataset loader for 2D LiDAR person detection.

The JackRabbot Dataset and Benchmark (JRDB) was collected at Stanford University
with a JackRabbot social robot carrying two SICK LMS 500 laser scanners.

  Martín-Martín, Rezatofighi et al.
  "JRDB: A Dataset and Benchmark of Egocentric Robot Visual Perception
   of Humans in Built Environments"
  IEEE TPAMI, 2021 — arXiv:1910.11792
  https://jrdb.erc.monash.edu/

MANUAL DOWNLOAD REQUIRED
------------------------
JRDB requires free registration at https://jrdb.erc.monash.edu/ before data
can be downloaded.  Only the 2D LiDAR subset is needed for this class.

After downloading, pre-process the data to DROW-compatible format using the
script from the 2D_lidar_person_detection repository:
  https://github.com/VisualComputingInstitute/2D_lidar_person_detection

Expected directory layout (same CSV + annotation format as DROW):
  <data_root>/
    train/          ← or whatever split names you choose
      sequence_0001.bag.csv
      sequence_0001.bag.wp
      sequence_0001.bag.odom2
      ...
    val/
      ...

Sensor properties (SICK LMS 500)
---------------------------------
  N_BEAMS        = 541
  FOV            = 270°  (−135° … +135°)
  LASER_INCREMENT = 0.5° per beam  (π/360 rad)
  LASER_MIN_ANGLE = −135° (−3π/4 rad)
  LASER_MAX_ANGLE = +135° (+3π/4 rad)

Annotation density
------------------
Unlike DROW (1 annotated frame per 5 scans), JRDB provides dense annotations
for every scan frame.  Consequently idet2iscan is a 1-to-1 mapping and
``time_frame`` is set to 5 for temporal history consistency with other loaders.

NOTE: JRDB test-split labels are withheld for the benchmark leaderboard and
are not available for local evaluation.  Only use the train split (possibly
held out as val) for supervised training and evaluation.
"""

import warnings
from json import loads
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Union

import numpy as np
from numpy import (
    arange, array, array_equal, concatenate, float32, fromregex,
    genfromtxt, linspace, repeat, tile, uint32, vectorize, where,
)
from numpy.typing import NDArray

from ..utils.file_utils import JRDB_DATA_PATH
from ..utils.generic_utils import Logging

_DATASET_PATH = Path(__file__).parent.parent / JRDB_DATA_PATH

# Sensor geometry
_N_BEAMS        = 541
_FOV_DEG        = 270.0
_INC_RAD        = float(np.radians(_FOV_DEG) / (_N_BEAMS - 1))   # 0.5° in rad
_MIN_ANGLE_RAD  = float(-np.radians(_FOV_DEG / 2))               # -135°
_MAX_ANGLE_RAD  = float(+np.radians(_FOV_DEG / 2))               # +135°


def jrdb_laser_angles(n_beams: int = _N_BEAMS) -> NDArray:
    """
    Return beam angles for the JRDB SICK LMS 500 scanner.

    Parameters
    ----------
    n_beams : number of beams (default 541)

    Returns
    -------
    (n_beams,) float64 — angles in radians, from −135° to +135°
    """
    return linspace(_MIN_ANGLE_RAD, _MAX_ANGLE_RAD, n_beams, dtype=np.float64)


class JRDB_Dataset(Logging):
    """
    2D LiDAR person detection dataset wrapper for JRDB (DROW-format files).

    Matches the interface of DROW_Dataset and FROG_Dataset:
      ``get_scan(seq_id, scan_id, time_window)`` → (scans, odoms)

    Parameters
    ----------
    datapath       : root directory of the pre-processed JRDB data
    split          : subdirectory name identifying the split (default "train")
    verbose        : print progress messages (default True)

    Raises
    ------
    FileNotFoundError
        If no .csv files are found under ``datapath / split``.  This usually
        means the data has not been downloaded or pre-processed yet.
    """

    TIME_FRAME      = 5
    N_BEAMS         = _N_BEAMS
    LASER_MIN_ANGLE = _MIN_ANGLE_RAD
    LASER_MAX_ANGLE = _MAX_ANGLE_RAD
    LASER_INCREMENT = _INC_RAD

    _LOAD_JSON_VECTOR = vectorize(loads, otypes=[object])

    def __init__(
        self,
        datapath:  Union[Path, str] = _DATASET_PATH,
        split:     Union[Path, str] = "train",
        verbose:   bool             = True,
    ):
        Logging.__init__(self, verbose)

        dataset_path = Path(datapath) / Path(split)
        if not dataset_path.exists():
            raise FileNotFoundError(
                f"JRDB dataset directory not found: {dataset_path}\n\n"
                "JRDB requires manual download and pre-processing:\n"
                "  1. Register (free) at https://jrdb.erc.monash.edu/\n"
                "  2. Download the 2D LiDAR subset\n"
                "  3. Pre-process to DROW format using:\n"
                "     https://github.com/VisualComputingInstitute/2D_lidar_person_detection\n"
                f"  4. Place the '{split}/' directory under: {datapath}"
            )

        csv_files = sorted(dataset_path.glob("*.bag.csv"))
        if not csv_files:
            raise FileNotFoundError(
                f"No .bag.csv files found in {dataset_path}.\n"
                "Make sure the JRDB data has been pre-processed to DROW format."
            )

        self.time_frame = self.TIME_FRAME
        self.filenames  = [f"{f.parent}/{f.stem[:-4]}"  # strip ".bag" suffix
                           for f in csv_files]
        # Keep ".bag" in the base to match annotation file naming
        self.filenames  = [str(f.parent / f.stem[:-len(".bag.csv") - 1 + 4])
                           for f in csv_files]
        # Simpler: just strip the final extension (.csv)
        self.filenames  = [str(f)[:-4] for f in csv_files]  # path without .csv

        self._print(f"JRDB {split}: found {len(self.filenames)} sequence(s)")

        # ── Scan data ────────────────────────────────────────────────────────
        self.scan_id:  NDArray[uint32]
        self.scan_time: NDArray[float32]
        self.scans:    NDArray[float32]

        scan_data = array(
            [self._load_scan(f + ".csv", _N_BEAMS) for f in self.filenames],
            dtype=object,
        )
        self.scan_id, self.scan_time, self.scans = scan_data.transpose()
        self._print(f"  Scans loaded ({_N_BEAMS} beams each)")

        # ── Annotation data ──────────────────────────────────────────────────
        # JRDB annotations are person-only (.wp).
        # We create empty wc/wa arrays to maintain interface compatibility.
        self.det_id: NDArray[uint32]
        self.det_wc: NDArray
        self.det_wa: NDArray
        self.det_wp: NDArray

        wp_id, self.det_wp = array(
            [self._load_det(f + ".wp") for f in self.filenames], dtype=object
        ).transpose()

        # If .wc / .wa files exist use them; otherwise create empty annotations
        wc_data = self._try_load_class(self.filenames, ".wc", wp_id)
        wa_data = self._try_load_class(self.filenames, ".wa", wp_id)
        wc_id, self.det_wc = wc_data
        wa_id, self.det_wa = wa_data

        self.det_id = wp_id
        self._print(f"  Annotations loaded ({sum(len(d) for d in self.det_wp)} person instances)")

        # ── Odometry ─────────────────────────────────────────────────────────
        self.odoms:      NDArray
        self.idet2iscan: List[Dict[int, int]]

        self.odoms = array(
            [self._load_odom(f + ".odom2") for f in self.filenames], dtype=object
        )

        # JRDB has dense annotations (1:1 with scan frames).
        # idet2iscan maps detection_index → scan_array_index.
        self.idet2iscan = [
            {i: where(sid == d)[0][0]
             for i, d in enumerate(did)}
            for sid, did in zip(self.scan_id, self.det_id)
        ]
        self._print(f"  Odometry loaded\n")

    # ------------------------------------------------------------------
    # Private loaders (identical to DROW_Dataset implementations)
    # ------------------------------------------------------------------

    @staticmethod
    def _load_scan(
        fname: Union[Path, str], n_beams: int
    ) -> Tuple[NDArray, NDArray, NDArray]:
        return genfromtxt(
            fname, delimiter=",",
            dtype=[("id", uint32), ("time", float32), ("scan", float32, n_beams)],
            unpack=True,
        )

    @classmethod
    def _load_det(
        cls, fname: Union[Path, str]
    ) -> Tuple[NDArray, NDArray]:
        data = fromregex(
            fname, r"(\d+),([^\n]+)",
            dtype=[("id", uint32), ("json", object)],
        )
        return data["id"], cls._LOAD_JSON_VECTOR(data["json"])

    @staticmethod
    def _load_odom(fname: Union[Path, str]) -> NDArray:
        return genfromtxt(
            fname, delimiter=",",
            dtype=[("eq", uint32), ("t", float32), ("xya", float32, 3)],
        )

    @classmethod
    def _try_load_class(
        cls,
        filenames: List[str],
        ext:       str,
        fallback_ids: NDArray,
    ) -> Tuple[NDArray, NDArray]:
        """Load an optional annotation class; return empty arrays if not found."""
        ids_list, dets_list = [], []
        for f, fid in zip(filenames, fallback_ids):
            path = Path(f + ext)
            if path.exists():
                fid_loaded, det_loaded = cls._load_det(str(path))
                ids_list.append(fid_loaded)
                dets_list.append(det_loaded)
            else:
                # Create empty annotations with the same IDs as the wp file
                ids_list.append(fid)
                dets_list.append(array([[] for _ in fid], dtype=object))
        return array(ids_list, dtype=object), array(dets_list, dtype=object)

    # ------------------------------------------------------------------
    # Public interface (mirrors DROW_Dataset)
    # ------------------------------------------------------------------

    def get_scan(
        self,
        sequence_id: int,
        scan_id:     int,
        time_window: int,
    ) -> Tuple[NDArray, NDArray]:
        """
        Extract a temporal window of scans and odometry ending at ``scan_id``.

        Parameters
        ----------
        sequence_id : index into the sequence list
        scan_id     : index of the target scan within the sequence
        time_window : number of consecutive scans to return (T)

        Returns
        -------
        scans : (T, N_BEAMS) float32
        odoms : (T,) structured array with fields eq, t, xya
        """
        start = scan_id - time_window + 1
        if start < 0:
            pad    = abs(start)
            scans  = concatenate([
                tile(self.scans[sequence_id][0], (pad, 1)),
                self.scans[sequence_id][: scan_id + 1],
            ])
            odoms  = concatenate([
                repeat(self.odoms[sequence_id][:1], pad, axis=0),
                self.odoms[sequence_id][: scan_id + 1],
            ])
        else:
            scans = self.scans[sequence_id][start: scan_id + 1]
            odoms = self.odoms[sequence_id][start: scan_id + 1]
        return scans, odoms
