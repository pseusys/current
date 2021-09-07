from typing import Callable

from scipy.integrate import quad
from scipy.optimize import fsolve
from numpy.linalg import det
from numpy import inf
import pandas as pd


# Интеграл -
def integr(f: Callable[[float], float], lower: float = -inf, upper: float = +inf) -> (float, float):
    return quad(f, lower, upper)


# Определитель -
def determinant(frame: pd.DataFrame) -> float:
    return det(frame.values)


def intersection(f1: Callable[[float], float], f2: Callable[[float], float], near: float = 0) -> [float]:
    return fsolve(lambda x: f1(x) - f2(x), near)
