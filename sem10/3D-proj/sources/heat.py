from bisect import bisect_left
from random import uniform

import numpy as np

from sources.utils import lerp


_TERRAIN_COLORS = [
    np.array([(.4, .4, .2), (.8, 1., 1.), (.4, .4, .2), (.2, .2, .0), (.4, .4, .6), (1., 1., 1.)], dtype=np.float64),
    np.array([(.6, .6, .4), (.8, .6, .0), (.0, .4, .0), (.2, .2, .0), (.6, .6, .4), (.8, .8, .8)], dtype=np.float64),
    np.array([(.8, .8, .0), (1., 1., .4), (.0, .8, .0), (.2, .2, .0), (.4, .2, .0), (.6, .4, .0)], dtype=np.float64)
]

_TERRAIN_SHININESS = [
    np.array([(0.,), (0.,), (.1,), (.1,), (.4,), (.8,)], dtype=np.float64),
    np.array([(0.,), (0.,), (0.,), (0.,), (.1,), (.4,)], dtype=np.float64),
    np.array([(0.,), (0.,), (0.,), (0.,), (0.,), (0.,)], dtype=np.float64)
]

_TREE_COLORS = [
    np.array((.0, .0, .4), dtype=np.float64),
    np.array((.4, .2, 0), dtype=np.float64),
    np.array((.0, .4, 0), dtype=np.float64)
]


class Heat:
    RANDOM = -1
    TEMPERATURES = [0, 1, 2]

    def __init__(self, heat_state: float):
        if heat_state < 0 and heat_state == self.RANDOM:
            heat_state = uniform(self.TEMPERATURES[0], self.TEMPERATURES[-1])
        elif not (self.TEMPERATURES[0] <= heat_state <= self.TEMPERATURES[-1]):
            raise RuntimeError(f"Unknown heat value! Must be in [{self.TEMPERATURES[0]}; {self.TEMPERATURES[-1]}] or {self.RANDOM} for random!")

        self.generate_ice = heat_state < self.TEMPERATURES[1]
        index = bisect_left(self.TEMPERATURES, heat_state)
        fraction = (heat_state - self.TEMPERATURES[index - 1]) / (self.TEMPERATURES[index] - self.TEMPERATURES[index - 1])
        self.terrain_colors = lerp(_TERRAIN_COLORS[index - 1], _TERRAIN_COLORS[index], fraction)
        self.terrain_shininess = lerp(_TERRAIN_SHININESS[index - 1], _TERRAIN_SHININESS[index], fraction)
        self.tree_colors = lerp(_TREE_COLORS[index - 1], _TREE_COLORS[index], fraction)
