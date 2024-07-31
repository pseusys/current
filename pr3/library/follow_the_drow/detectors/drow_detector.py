"""
This file is totally based on research papaer about DROW.
Here th link to the paper can be found:
https://arxiv.org/abs/1603.02636
This model is based on the "final best model" described in paper:
https://github.com/VisualComputingInstitute/DROW/blob/master/v2/Clean%20Final*%20%5BT%3D5%2Cnet%3Ddrow3xLF2p%2Codom%3Drot%2Ctrainval%5D.ipynb
"""

from pathlib import Path
from time import time
from typing import Union

from torch import no_grad, from_numpy, sum as tsum, device, load
from torch.nn import Module, Conv1d, BatchNorm1d
from torch.nn.functional import leaky_relu, max_pool1d, dropout, avg_pool1d, softmax
from torch.nn.init import kaiming_normal_, constant_
from torch.backends import cudnn
from torch.autograd import Variable

from numpy import array

from tqdm.auto import trange

from .detector import Detector
from ..datasets import DROW_Dataset
from ..utils.file_utils import DROW_WEIGHTS_PATH
from ..utils.drow_utils import cutout
from ..utils.torch_utils import init_module, move, count_parameters

cudnn.benchmark = True  # Run benchmark to select fastest implementation of ops.

_DETECTOR_WEIGHTS_PATH = Path(__file__).parent.parent / DROW_WEIGHTS_PATH


class DrowDetector(Module, Detector):
    N_SAMP = 48
    GPU = False  # This is the GPU index, use 0 for first GPU.

    def __init__(self, dropout: float, time_frame_size: int, verbose: bool, *args, **kwargs):
        Module.__init__(self, *args, **kwargs)
        Detector.__init__(self, verbose)
        self.time_frame = time_frame_size

        self.dropout = dropout
        self.conv1a = Conv1d(1, 64, kernel_size=3, padding=1)
        self.bn1a = BatchNorm1d( 64)
        self.conv1b = Conv1d(64, 64, kernel_size=3, padding=1)
        self.bn1b = BatchNorm1d(64)
        self.conv1c = Conv1d(64, 128, kernel_size=3, padding=1)
        self.bn1c = BatchNorm1d(128)
        self.conv2a = Conv1d(128, 128, kernel_size=3, padding=1)
        self.bn2a = BatchNorm1d(128)
        self.conv2b = Conv1d(128, 128, kernel_size=3, padding=1)
        self.bn2b = BatchNorm1d(128)
        self.conv2c = Conv1d(128, 256, kernel_size=3, padding=1)
        self.bn2c = BatchNorm1d(256)
        self.conv3a = Conv1d(256, 256, kernel_size=3, padding=1)
        self.bn3a = BatchNorm1d(256)
        self.conv3b = Conv1d(256, 256, kernel_size=3, padding=1)
        self.bn3b = BatchNorm1d(256)
        self.conv3c = Conv1d(256, 512, kernel_size=3, padding=1)
        self.bn3c = BatchNorm1d(512)
        self.conv4a = Conv1d(512, 256, kernel_size=3, padding=1)
        self.bn4a = BatchNorm1d(256)
        self.conv4b = Conv1d(256, 128, kernel_size=3, padding=1)
        self.bn4b = BatchNorm1d(128)
        self.conv4p = Conv1d(128, 4, kernel_size=1)  # probs
        self.conv4v = Conv1d(128, 2, kernel_size=1)  # vote

        self.reset_parameters()

    @classmethod
    def init(cls, weights_file: Union[Path, str] = _DETECTOR_WEIGHTS_PATH, dropout: float = 0.5, time_frame_size: int = DROW_Dataset.TIME_FRAME, verbose: bool = True) -> "DrowDetector":
        map_location = None if cls.GPU else device("cpu")
        weights = load(weights_file, map_location)

        detector = move(cls(dropout=dropout, time_frame_size=time_frame_size, verbose=verbose), cls.GPU)
        detector.reset_parameters()
        detector.load_state_dict(weights["model"])
        detector._print(f"Loaded weights from {weights_file}")
        return detector

    @property
    def parameters_number(self) -> int:
        return count_parameters(self)

    def forward(self, x):
        (B, T), R = x.shape[:2], x.shape[2:]
        x = x.view(B*T, 1, *R)

        x = leaky_relu(self.bn1a(self.conv1a(x)), 0.1)
        x = leaky_relu(self.bn1b(self.conv1b(x)), 0.1)
        x = leaky_relu(self.bn1c(self.conv1c(x)), 0.1)
        x = max_pool1d(x, 2)  # 24
        x = dropout(x, p=self.dropout, training=self.training)

        x = leaky_relu(self.bn2a(self.conv2a(x)), 0.1)
        x = leaky_relu(self.bn2b(self.conv2b(x)), 0.1)
        x = leaky_relu(self.bn2c(self.conv2c(x)), 0.1)
        x = max_pool1d(x, 2)  # 12
        x = dropout(x, p=self.dropout, training=self.training)

        x = x.view(B, T, *x.shape[1:])
        x = tsum(x, dim=1)

        x = leaky_relu(self.bn3a(self.conv3a(x)), 0.1)
        x = leaky_relu(self.bn3b(self.conv3b(x)), 0.1)
        x = leaky_relu(self.bn3c(self.conv3c(x)), 0.1)
        x = max_pool1d(x, 2)  # 6
        x = dropout(x, p=self.dropout, training=self.training)

        x = leaky_relu(self.bn4a(self.conv4a(x)), 0.1)
        x = leaky_relu(self.bn4b(self.conv4b(x)), 0.1)
        x = avg_pool1d(x, 6)

        logits = self.conv4p(x)
        votes = self.conv4v(x)
        return logits[:, :, 0], votes[:, :, 0]  # Due to the arch, output has spatial size 1, so we [0] it.

    def reset_parameters(self):
        init_module(self.conv1a, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv1b, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv1c, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv2a, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv2b, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv2c, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv3a, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv3b, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv3c, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv4a, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv4b, lambda t: kaiming_normal_(t, a=0.1), 0)
        init_module(self.conv4p, lambda t: constant_(t, 0), 0)
        init_module(self.conv4v, lambda t: constant_(t, 0), 0)
        constant_(self.bn1a.weight, 1)
        constant_(self.bn1b.weight, 1)
        constant_(self.bn1c.weight, 1)
        constant_(self.bn2a.weight, 1)
        constant_(self.bn2b.weight, 1)
        constant_(self.bn2c.weight, 1)
        constant_(self.bn3a.weight, 1)
        constant_(self.bn3b.weight, 1)
        constant_(self.bn3c.weight, 1)
        constant_(self.bn4a.weight, 1)
        constant_(self.bn4b.weight, 1)

    def forward_one(self, xb):
        self.eval()
        with no_grad():
            logits, votes = self(Variable(move(from_numpy(xb), self.GPU)))
            return softmax(logits, dim=-1).data.cpu().numpy(), votes.data.cpu().numpy()

    def forward_all(self, va: DROW_Dataset):
        times = list()
        all_confs, all_votes = [], []
        for iseq in trange(len(va.det_id), desc="Sequences", disable=not self._verbose):
            for idet in trange(len(va.det_id[iseq]), desc="Scans", disable=not self._verbose, leave=False):
                start_time = time()
                iscan = va.idet2iscan[iseq][idet]
                scans, odoms = va.get_scan(iseq, iscan, self.time_frame)
                cut = cutout(scans, odoms, len(va.scans[iseq][iscan]), nsamp=self.N_SAMP)
                confs, votes = self.forward_one(cut)
                all_confs.append(confs)
                all_votes.append(votes)
                times += [time() - start_time]
        self._print(f"Average detection time: {sum(times) / len(times)} seconds")
        return array(all_confs), array(all_votes)
