from math import exp, pi, sqrt

import numpy as np
import numpy.typing as npt

from .transform import identity


normal_normal = np.array([0.0, 1.0, 0.0], dtype=np.float64)


def triangle_normal(v1, v2, v3):
    v1 = np.array(v1, dtype=np.float64)
    norm = np.cross(np.array(v2, dtype=np.float64) - v1, np.array(v3, dtype=np.float64) - v1)
    return norm / np.linalg.norm(norm)


def laplacian_of_gaussian(x: float, z: float, sigma: float = 1., width: float = .3, height: float = 20., flatness: float = 100.) -> float:
    term = -width * (x ** 2 + z ** 2) / (2 * sigma ** 2)
    return -(1 + term * height) * exp(term) / (pi * sigma ** 4) * flatness


def sign(x: float):
    return 1 if x > 0 else -1


def square_extended(x: float, z: float, shore_size: int = 40., dive: int = 50.) -> float:
    dist = sqrt(x ** 2 + z ** 2)
    return sqrt(max(dist - shore_size, 0)) / dive


def straight_angle_rotor(distance: npt.NDArray[np.float64]) -> npt.NDArray[np.float64]:
    x = np.random.randn(3)
    x -= x.dot(distance) * distance
    x /= np.linalg.norm(x)
    y = np.cross(distance, x)
    return [x, y, -x, -y]


def normalize(vector: npt.NDArray[np.float64]):
    return vector / np.linalg.norm(vector)


def find_normal_rotation(start: npt.NDArray[np.float64], end: npt.NDArray[np.float64]):
    start = normalize(start)
    end = normalize(end)
    v = np.cross(start, end)
    cosine = np.dot(start, end)
    matrix = np.array([
        [0,    -v[2], v[1],  0],
        [v[2],  0,    -v[0], 0],
        [-v[1], v[0], 0,     0],
        [0,     0,    0,     1]
    ], 'f')
    return identity() + matrix + (matrix @ matrix) * (1 / (1 + cosine))
