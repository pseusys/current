from bisect import bisect_left
from typing import Callable, Optional, Tuple

import numpy as np
import numpy.typing as npt

from ..wrapper import Shader, Mesh
from ..utils import triangle_normal, lerp, normalize


_EDGE_MAP = np.array([.0, .1, .2, .5, .9, 1.1], dtype=np.float64)
_COLOR_MAP = np.array([(.8, .8, .0), (1., 1., .4), (.0, .8, .0), (.2, .2, .0), (.6, .6, .4), (.8, .8, .8)], dtype=np.float64)
_SHINY_MAP = np.array([(0.,), (0.,), (1.,), (0.,), (4.,), (32.,)], dtype=np.float64)


def _flat_gen(_: int, __: int) -> float:
    return 0.0


class Terrain(Mesh):
    def __init__(self, shader: Shader, size_x: int, size_z: int, radius: int, color_map: npt.NDArray[np.float64] = _COLOR_MAP, shiny_map: npt.NDArray[np.float64] = _SHINY_MAP, generator: Optional[Callable[[int, int], float]] = None):
        if size_x <= 1 or size_z <= 1:
            raise Exception(f"Both terrain length ({size_x}) and width ({size_z}) should be greater than one!")

        self._x = size_x
        self._z = size_z
        self._radius = radius
        generator = _flat_gen if generator is None else generator

        # Calculate vertex positions:

        self._position = [None for _ in range(size_x * size_z)]
        for x in range(size_x):
            for z in range(size_z):
                self._position[x * size_x + z] = (-size_x // 2 + x, generator(x, z), -size_z // 2 + z)
        self._position = np.array(self._position, dtype=np.float64)

        # Calculate triangle indexes together with normals:

        index = list()
        self._normals = [list() for _ in range(size_x * size_z)]
        for x in range(size_x - 1):
            for z in range(size_z - 1):
                index += self._compute_index_and_normals(x, size_x, z)

        # Calculate average normal for every vertex

        index = np.array(index, dtype=np.uint32)
        for idx in range(len(self._normals)):
            self._normals[idx] = np.average(self._normals[idx], axis=0)

        # Calculate lower and higher height maps

        self._minimal_height = np.amin(self._position, axis=0)[1]
        self._maximal_height = np.amax(self._position, axis=0)[1]
        self._height_difference = self._maximal_height - self._minimal_height

        # Calculate material color depending on height

        k_d = [None for _ in range(size_x * size_z)]
        s = [None for _ in range(size_x * size_z)]
        for idx in range(len(self._position)):
            k_d[idx], s[idx] = self._get_edge_index_fraction(self._position[idx][1], color_map, shiny_map)

        k_a = np.array([(0, 0, 0) for _ in range(size_x * size_z)], dtype=np.float64)
        k_s = np.array([(1, 1, 1) for _ in range(size_x * size_z)], dtype=np.float64)
        a = np.array([(1,) for _ in range(size_x * size_z)], dtype=np.float64)

        # Set variables

        colors = dict(k_a=k_a, k_d=np.array(k_d, dtype=np.float64), k_s=k_s, s=np.array(s, dtype=np.float64), a=a)
        attributes = dict(position=self._position, normal=self._normals, **colors)
        super().__init__(shader, index=index, attributes=attributes)

    def _compute_index_and_normals(self, x, size_x, z) -> Tuple[float, float, float, float, float, float]:
        vci = x * size_x + z  # Vertex current index
        vbi = x * size_x + (z + 1)  # Vertex below index
        vri = (x + 1) * size_x + z  # Vertex right index
        voi = (x + 1) * size_x + (z + 1)  # Vertex opposite index

        odd_triangle = (vci, vbi, voi)
        even_triangle = (voi, vri, vci)
        index = odd_triangle + even_triangle

        odd_normal = triangle_normal(*[self._position[i] for i in odd_triangle])
        even_normal = triangle_normal(*[self._position[i] for i in even_triangle])

        for vertex in odd_triangle:
            self._normals[vertex] += [odd_normal]
        for vertex in even_triangle:
            self._normals[vertex] += [even_normal]
        return index

    def _get_edge_index_fraction(self, height: float, color_map: npt.NDArray[np.float64], shiny_map: npt.NDArray[np.float64]) -> Tuple[npt.NDArray[np.float64], float]:
        height = (height - self._minimal_height) / self._height_difference
        index = bisect_left(_EDGE_MAP, height)
        fraction = (height - _EDGE_MAP[index - 1]) / (_EDGE_MAP[index] - _EDGE_MAP[index - 1])
        color = lerp(color_map[index - 1], color_map[index], fraction)
        shininess = lerp(shiny_map[index - 1], shiny_map[index], fraction)
        return color, shininess

    def get_normal(self, x: int, z: int) -> npt.NDArray[np.float64]:
        return normalize(self._normals[x * self._x + z])

    def get_position(self, x: int, z: int) -> npt.NDArray[np.float64]:
        return self._position[x * self._x + z]
