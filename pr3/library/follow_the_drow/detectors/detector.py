from abc import ABC, abstractmethod

from ..datasets import DROW_Dataset
from ..utils.generic_utils import Logging


class Detector(Logging, ABC):
    def __init__(self, verbose: bool = True):
        Logging.__init__(self, verbose)

    @classmethod
    @abstractmethod
    def init(cls, verbose: bool = True) -> "Detector":
        raise NotImplementedError

    @abstractmethod
    def forward_one(self, xb):
        raise NotImplementedError

    @abstractmethod
    def forward_all(self, va: DROW_Dataset):
        raise NotImplementedError
