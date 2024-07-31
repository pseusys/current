from random import randint
from typing import Callable, List, Tuple

import numpy as np


def conditional_random_points(size: int, condition: Callable[[int, int], bool], ux: int = 0, uz: int = 0, lx: int = 10, lz: int = 10) -> List[Tuple[int, int]]:
    result = list()
    while len(result) < size:
        newcomer_x, newcomer_z = randint(lx, ux), randint(lz, uz)
        if condition(newcomer_x, newcomer_z):
            result += [(newcomer_x, newcomer_z)]
    return result


def terrain_generator(carrier_func: Callable[[float, float], float], noise_func: Callable[[float, float], float], size_x: int, size_z: int, carrier_value: float = 5., noise_value: float = 1.):
    carrier = [None for _ in range(size_x * size_z)]
    noise = [None for _ in range(size_x * size_z)]

    for x in range(-size_x // 2, size_x // 2):
        for z in range(-size_z // 2, size_z // 2):
            idx = (x + size_x // 2) * size_x + (z + size_z // 2)
            carrier[idx] = carrier_func(x, z)
            noise[idx] = noise_func(x / size_x, z / size_z)

    height_matrix = carrier_value * np.array(carrier, dtype=np.float64) + noise_value * np.array(noise, dtype=np.float64)

    def generator(xc: int, zc: int) -> float:
        return height_matrix[xc * size_x + zc]

    return generator
