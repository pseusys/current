from random import uniform, choice
from math import sin, cos
from typing import Tuple

import numpy as np
from OpenGL import GL

from ..utils import lerp
from ..wrapper import Mesh, Shader


class Smoke(Mesh):
    _WIDE_SPREAD = .6
    _SPEED_STEP = .001
    
    def __init__(self, shader: Shader, lower: int, higher: int, number: int = 1000, size: int = 2, thin: float = .5, thick: float = 1., slow: float = 1., fast: float = 5., weak: float = 10., powerful: float = 100., flight: float = 25.):
        GL.glPointSize(size)
        self.lower = lower
        self.higher = higher
        self.number = number
        self.flight = flight

        self.rotors = [uniform(thin, thick) * choice([-1, 1]) for _ in range(number)]
        self.speeds = [uniform(slow, fast) for _ in range(number)]
        self.ascends = [uniform(weak, powerful) for _ in range(number)]
        self.heights = [0 for _ in range(number)]

        self.coords = np.array([(0, 0, 0) for _ in range(number)], dtype=np.float32)
        self._update_coords(0)

        uniforms = dict(k_a=(0, 0, 0), k_d=(.4, .4, .4), a=.5) 
        super().__init__(shader, attributes=dict(position=self.coords), usage=GL.GL_STREAM_DRAW, **uniforms)

    def draw(self, primitives=GL.GL_POINTS, attributes=None, **uniforms):
        attributes = dict() if attributes is None else attributes
        self._update_coords(self._SPEED_STEP)
        attributes = dict(**attributes, position=self.coords)
        super().draw(primitives, attributes=attributes, **uniforms)

    def _update_coords(self, update_value: float):
        for i in range(self.number):
            self.heights[i] = (self.heights[i] + update_value * self.speeds[i]) % 1
            x, z = self._fog_position(self.heights[i], self.rotors[i], self.ascends[i])
            self.coords[i] = (x * self.flight, lerp(self.lower, self.higher, self.heights[i]), z * self.flight)

    def _fog_position(self, height: float, rotor: float, ascend: float) -> Tuple[float, float]:
        multi = rotor * height ** self._WIDE_SPREAD
        return multi * sin(height * ascend), multi * cos(height * ascend)
