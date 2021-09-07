from typing import Callable

import matplotlib.pyplot as plt
from numpy import arange

import pandas as pd

import matplotlib
matplotlib.use('TkAgg')


def scatter(frame: pd.DataFrame, column1: str = 0, column2: str = 0):
    frame.plot.scatter(column1, column2)
    plt.show(block=False)


def n_dist(frame: pd.DataFrame, column: str = 0):
    frame[column].plot.kde()
    plt.show(block=False)


def draw(f: Callable[[float], float], start: float = 0, end: float = 100, step: float = 1, cont: bool = False):
    if not cont:
        plt.figure()
    arr = [x for x in arange(start, end, step)]
    plt.plot(arr, [f(x) for x in arr])
    plt.show(block=False)


def flush(block: bool = True):
    plt.show(block=block)
