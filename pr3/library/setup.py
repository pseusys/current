from pathlib import Path
from urllib.request import urlretrieve
from zipfile import ZipFile
from setuptools import setup

from pybind11.setup_helpers import Pybind11Extension, build_ext

VERSION = "0.0.1"
DROW_DATA       = "https://github.com/VisualComputingInstitute/DROW/releases/download/v2/DROWv2-data.zip"
DROW_WEIGHTS    = "https://github.com/VisualComputingInstitute/DROW/releases/download/v2/final-WNet3xLF2p-T5-odom.rot-trainval-50ep.pth.tar"
DRSPAAM_WEIGHTS = "https://github.com/VisualComputingInstitute/DR-SPAAM-Detector/releases/download/v1.1/dr_spaam_e40.pth"
LFE_PEAKS_WEIGHTS = "https://robotics.upo.es/~famozur/onnx/LFE-Peaks.onnx"
LFE_PPN_WEIGHTS   = "https://robotics.upo.es/~famozur/onnx/LFE-PPN.onnx"

ext_modules = [
    Pybind11Extension(
        "follow_the_drow.cpp_binding",
        sources=[str(file) for file in Path("cpp_core/sources").glob("*.cpp")],
        include_dirs=[str(Path("cpp_core/include/follow_the_drow"))],
        define_macros=[("VERSION_INFO", VERSION)],
    ),
]



include_dir = Path("follow_the_drow/include")
if not include_dir.exists():
    include_dir.mkdir(parents=True)
    file, _ = urlretrieve(DROW_DATA)
    with ZipFile(file, "r") as archive:
        archive.extractall(include_dir)
    urlretrieve(DROW_WEIGHTS, include_dir / Path("weights.pth.tar"))
    urlretrieve(DRSPAAM_WEIGHTS, include_dir / Path("drspaam_weights.pth"))
    urlretrieve(LFE_PEAKS_WEIGHTS, include_dir / Path("lfe_peaks.onnx"))
    urlretrieve(LFE_PPN_WEIGHTS,   include_dir / Path("lfe_ppn.onnx"))
    open(include_dir / Path("__init__.py"), "w").close()

setup(
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
