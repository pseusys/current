from itertools import product
from math import floor, ceil
from random import randint
from typing import Optional, Tuple

import numpy as np
import numpy.typing as npt

from .np_misc import cached, vectorized


@cached()
def _create_sample(variation: npt.NDArray[np.uint64], seed: int) -> Tuple[npt.NDArray[np.float64], npt.NDArray[np.uint64]]:
    hashed_coords = abs(int(hash(variation.tobytes())))
    return 2 * np.random.default_rng(seed * hashed_coords).random(variation.size) - 1


@vectorized
def _fade(given_value: np.float64) -> np.float64:
    given_value = 1 - abs(given_value)
    if given_value < -0.1 or given_value > 1.1:
        raise ValueError('expected to have value in [-0.1, 1.1]')
    return 6 * given_value ** 5 - 15 * given_value ** 4 + 10 * given_value ** 3


def _get_noise_value(variation: npt.NDArray[np.uint64], seed: int, coordinates: npt.NDArray[np.uint64]):
    random_sample = _create_sample(variation, seed)
    weight = _fade(coordinates - variation).prod()
    return weight * (random_sample @ (coordinates - variation))


@cached(keys=("x", "y"))
def noise(x: int, y: int, octaves: int = 15, seed: Optional[int] = None) -> float:
    seed = randint(1, 10**5) if seed is None else seed
    coords = np.array([x * octaves, y * octaves], np.float64)
    bounds = [(floor(coord), ceil(coord)) for coord in coords]
    permutations = np.array(list(product(*bounds)), np.float64)
    return sum(_get_noise_value(item, seed, coords) for item in permutations)
