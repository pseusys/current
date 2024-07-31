from math import floor
from random import uniform
import OpenGL.GL as GL
import numpy as np
import numpy.typing as npt

from .image import Image
from ..time import Chronograph
from ..wrapper import Shader, Node
from ..heat import Heat
from ..utils import MeshedNode, lerp, straight_angle_rotor, normal_normal, identity, normalize, translate, find_normal_rotation, rotate


_COLOR_MAP = np.array((.666, .333, 0), dtype=np.float64)
_NORMAL_FORWARD = np.array([1.0, 0.0, 0.0], dtype=np.float64)


def _sample() -> float:
    return uniform(0, 1)


class EmptyTree(MeshedNode):
    def __init__(self, shader: Shader, height: float = 3., radius: float = 1., color_map: npt.NDArray[np.float64] = _COLOR_MAP, transform=identity(), **_):
        self.rotor = straight_angle_rotor(normal_normal)
        self.normals = [normal_normal] + self.rotor
        self.position = [normal_normal * height] + [normal * radius for normal in self.rotor]
        self.index = [0, 4, 1, 0, 1, 2, 0, 2, 3, 0, 3, 4]

        self._a = np.array([(1,) for _ in range(len(self.position))], dtype=np.float32)
        self._k_a = np.array([(0, 0, 0) for _ in range(len(self.position))], dtype=np.float32)
        self._k_d = np.array([color_map for _ in range(len(self.position))], dtype=np.float32)

        attributes = dict(position=self.position, normal=self.normals, k_a=self._k_a, k_d=self._k_d, a=self._a)
        super().__init__(shader, tuple(), transform, attributes, self.index)

    def draw(self, primitives=GL.GL_TRIANGLES, attributes=None, **uniforms):
        attributes = dict() if attributes is None else attributes
        attributes = dict(**attributes, k_a=self._k_a, k_d=self._k_d, a=self._a)
        super().draw(primitives, attributes, **uniforms)


class ColdTree(EmptyTree):
    _COLD_MULTIPLIER = 2

    def __init__(self, shader: Shader, height: float, radius: float = 1., circle: float = 3., color_map: npt.NDArray[np.float64] = _COLOR_MAP, transform=identity(), **_):
        self.height = height * self._COLD_MULTIPLIER
        self.radius = radius * self._COLD_MULTIPLIER
        super().__init__(shader, self.height, self.radius, color_map, transform)

        for rot in self.rotor:
            length = uniform(0.7, circle / 2)
            radius = uniform(0.5, 0.7)
            position = uniform(0.2, 0.7)

            direction = rot + 0.1 * np.array([_sample(), max(_sample() - .5, 0), _sample()], dtype=np.float64)
            rotation = find_normal_rotation(normal_normal, normalize(direction))
            retransform = translate(0, rot[1] + self.height * (1 - position), 0)

            twig = EmptyTree(shader, self.height * length, self.radius * radius, color_map, transform=retransform @ rotation)
            self.add(twig)


class MediumTree(MeshedNode):
    _TREE_TOP_STEPS = 4  # TODO: implement with sphere
    _TREE_TOP_COLOR = np.array([(.0, 1., .0, 1.), (.0, .4, .0, 1.), (1., .4, .0, 1.), (.0, .0, .0, .0)], dtype=np.float64)

    def __init__(self, shader: Shader, chrono: Chronograph, height: float, radius: float = 1., circle: float = 3., color_map: npt.NDArray[np.float64] = _COLOR_MAP, transform=identity(), **_):
        self.chrono = chrono
        self._parent = EmptyTree(shader, height, radius, color_map)

        circle_positions = [([0, height, 0] + normal * circle) for normal in self._parent.rotor]
        upper_positions = [pos + normal_normal * circle for pos in circle_positions]
        lower_positions = [pos - normal_normal * circle for pos in circle_positions]
        normals = [normalize(pos - normal_normal) for pos in upper_positions + lower_positions]
        position = upper_positions + lower_positions
        index = [
            3, 0, 2, 2, 0, 1,
            5, 4, 6, 6, 4, 7,
            7, 4, 3, 3, 4, 0,
            4, 5, 0, 0, 5, 1,
            5, 6, 1, 1, 6, 2,
            6, 7, 2, 2, 7, 3
        ]

        self._a = np.array([(1,) for _ in range(len(position))], dtype=np.float32)
        self._k_a = np.array([(0, 0, 0) for _ in range(len(position))], dtype=np.float32)
        self._k_d = np.array([color_map for _ in range(len(position))], dtype=np.float32)

        attributes = dict(position=position, normal=normals, k_a=self._k_a, k_d=self._k_d, a=self._a)
        super().__init__(shader, (self._parent,), transform, attributes, index)

    def draw(self, primitives=GL.GL_TRIANGLES, attributes=None, **uniforms):
        attributes = dict() if attributes is None else attributes
        current_season = (self.chrono.season - .5) if self.chrono.season - .5 > 0 else (3 + self.chrono.season - .5)
        next_season = (self.chrono.season + .5) if self.chrono.season + .5 < 3 else (3 - self.chrono.season - .5)
        fraction = current_season - floor(current_season)

        previous_color = self._TREE_TOP_COLOR[floor(current_season)][:-1]
        next_color = self._TREE_TOP_COLOR[floor(next_season)][:-1]
        season_color = lerp(previous_color, next_color, fraction)

        previous_alpha = self._TREE_TOP_COLOR[floor(current_season)][-1]
        next_alpha = self._TREE_TOP_COLOR[floor(next_season)][-1]
        season_alpha = lerp(previous_alpha, next_alpha, fraction)

        for i in range(len(self._k_a)):
            self._k_d[i] = season_color
            self._a[i] = (season_alpha,)

        attributes = dict(**attributes, k_a=self._k_a, k_d=self._k_d, a=self._a)
        super().draw(primitives, attributes, **uniforms)


class HotTree(Node):
    _LEAF_COUNT = 4

    def __init__(self, shader: Shader, leaf_shader: Shader, leaf_texture: str, height: float, radius: float = 1., circle: float = 3., color_map: npt.NDArray[np.float64] = _COLOR_MAP, transform=identity(), **_):
        Node.__init__(self, tuple(), transform)
        parent = EmptyTree(shader, height, radius, color_map)
        self.add(parent)

        length = height * uniform(0.7, circle / 2)
        for i in range(0, 360, 360 // self._LEAF_COUNT):
            rotation = rotate((0, 1, 0), i)
            retransform = translate(0, height, 0)
            rerotate = translate(0, 0, -length / 4)
            leaf = Image(leaf_shader, leaf_texture, (0, 0, 0), (.3, .3, .3), .1, 1., length, length / 2, False, retransform @ rotation @ rerotate)
            self.add(leaf)


def Tree(*args, heat_state: float, **kwargs):
    if round(heat_state) == Heat.TEMPERATURES[0]:
        return ColdTree(*args, **kwargs)
    elif round(heat_state) == Heat.TEMPERATURES[1]:
        return MediumTree(*args, **kwargs)
    elif round(heat_state) == Heat.TEMPERATURES[2]:
        return HotTree(*args, **kwargs)
    else:
        return EmptyTree(*args, **kwargs)
