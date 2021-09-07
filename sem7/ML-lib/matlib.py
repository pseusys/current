from math import exp, sqrt, pi
from typing import Callable

from supplib import det

import pandas as pd


# Среднее значение столбца - сумма всех чисел в нем, деленная на их количество (математическое ожидание)
def mean(frame: pd.DataFrame, column: str = 0) -> float:
    return frame[column].mean()


# Мода столбца -
def mode(frame: pd.DataFrame, column: str = 0) -> float:
    return frame[column].mode()[0]


# Медиана столбца -
def median(frame: pd.DataFrame, column: str = 0) -> float:
    return frame[column].median()


# Дисперсия столбца -
def var(frame: pd.DataFrame, column: str = 0) -> float:
    return frame[column].var()


# Среднеквадратичное отклонение столбца -
def std(frame: pd.DataFrame, column: str = 0) -> float:
    return frame[column].std()


def norm_dist(frame: pd.DataFrame, column: str = 0) -> Callable[[float], float]:
    return n_dist(std(frame, column), mean(frame, column))


# Нормальное распределение столбца -
def n_dist(sigma: float, mu: float) -> Callable[[float], float]:
    return lambda x: exp(-0.5 * ((x - mu) / sigma) ** 2) / (sigma * sqrt(2 * pi))


# Двумерное математическое ожидание -
def mean_2d(frame: pd.DataFrame, column1: str = 0, column2: str = 0) -> (float, float):
    return frame[column1].mean(), frame[column2].mean()


# Ковариационная матрица -
def cov(frame: pd.DataFrame) -> pd.DataFrame:
    return frame.cov()


# Обобщенная дисперсия -
def corr(frame: pd.DataFrame) -> float:
    return det(frame.cov())
