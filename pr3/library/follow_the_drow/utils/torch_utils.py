"""
This file is totally based on Lucas Eyer's toolbox library.
It is named `lbtoolbox`. Please, admire it with respect:
https://github.com/lucasb-eyer/lbtoolbox
"""

from typing import Optional, Union, Callable, TypeVar, Any

from torch import Tensor
from torch.nn import Module
from torch.nn.init import constant_
from torch.cuda import is_available

from .generic_utils import istype

_T = TypeVar("_T", Module, Tensor)


def init_module(mod: Module, weight: Optional[Union[Callable[[Tensor], Any], float]] = None, bias: int = 0) -> Module:
    """
    Original name: `init`
    """
    if weight is not None and istype(mod, "weight", Tensor):
        if callable(weight):
            weight(mod.weight)  # type: ignore
        else:
            constant_(mod.weight, weight)  # type: ignore
    if istype(mod, "bias", Tensor):
        constant_(mod.bias, bias)  # type: ignore
    return mod


def move(what: _T, use_cuda: Union[bool, int] = False, **kw) -> _T:
    """
    Original name: `maybe_cuda`
    """
    if use_cuda is True:
        use_cuda = 0
    if use_cuda is not False and is_available():
        what = what.cuda(device=use_cuda, **kw)
    return what


def count_parameters(net: Module) -> int:
    """
    Original name: `count_parameters`
    """
    return sum(map(lambda p: p.data.nelement(), net.parameters()))
