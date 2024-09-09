from collections import OrderedDict
from typing import Callable, List, Optional

import numpy as np


class lru_multi_cache:
    def __init__(self, original_function: Callable, maxsize: Optional[int], keys: Optional[List[str]]):
        self.original_function = original_function
        self.maxsize = maxsize
        self.keys = keys
        self.mapping = OrderedDict()

    def __call__(self, *args, **kwargs):
        considerable = args + tuple(value for key, value in kwargs.items() if key in self.keys)
        key = tuple(arg.tobytes() if isinstance(arg, np.ndarray) else hash(arg) for arg in considerable)
        try:
            value = self.mapping[key]
            self.mapping.move_to_end(key)
        except KeyError:
            value = self.original_function(*args, **kwargs)
            if self.maxsize is not None and len(self.mapping) >= self.maxsize:
                self.mapping.popitem(False)
            self.mapping[key] = value
        return value


def cached(keys: Optional[List[str]] = None, maxsize: Optional[int] = None):
    return lambda of: lru_multi_cache(of, maxsize, keys)


@cached(maxsize=1)
def vectorized(original_function: Callable):
    return np.vectorize(original_function, cache=True)
