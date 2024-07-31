from math import sin, pi, cos

import numpy as np

from sources.heat import Heat


class Chronograph:
    SEASON_LENGTH = 91
    YEAR_LENGTH = 364

    def __init__(self, heat_state: float, sun_bias: float = 10., init_day: float = SEASON_LENGTH, day_length: float = 10):
        self._day_length = day_length
        self._init_time = init_day * day_length
        self._global_time = self._init_time
        self._winter_bias = (Heat.TEMPERATURES[-1] - heat_state) * sun_bias

    @property
    def day(self) -> float:
        return self._global_time / self._day_length

    @property
    def season(self) -> float:
        return (self._global_time / (self.SEASON_LENGTH * self._day_length)) % 4

    @property
    def year(self) -> float:
        return self._global_time / (self.YEAR_LENGTH * self._day_length)

    @property
    def sun_position(self):
        escalation = pi * self.day
        bias = abs(cos(pi * (self.year + .125))) * self._winter_bias
        return np.array([bias, cos(escalation), sin(escalation)], dtype=np.float64)

    def update(self, global_time: float):
        self._global_time = global_time + self._init_time
