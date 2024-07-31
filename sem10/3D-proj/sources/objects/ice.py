import numpy as np

from ..utils import MeshedNode, identity
from ..wrapper import Shader


class Ice(MeshedNode):
    def __init__(self, shader: Shader, transform=identity(), **_):
        normals = [(-1, 1, 1), (-1, 1, -1), (1, 1, 1), (1, 1, -1), (-1, -1, 1), (-1, -1, -1), (1, -1, 1), (1, -1, -1)]
        position = [
            (-1, .25, 1),
            (-1, .25, -1),
            (1, .25, 1),
            (1, .25, -1),
            (-1, -.25, 1),
            (-1, -.25, -1),
            (1, -.25, 1),
            (1, -.25, -1)
        ]
        index = [
            0, 2, 1,
            1, 2, 3,
            0, 4, 6,
            0, 6, 2,
            2, 6, 7,
            2, 7, 3,
            3, 7, 5,
            3, 5, 1,
            1, 5, 4,
            1, 4, 0
        ]

        uniforms = dict(k_a=(0, 0, 0), k_d=(.8, 1., 1.), k_s=(0, 0, 0), s=1., a=.75)
        attributes = dict(position=position, normal=normals)
        super().__init__(shader, tuple(), transform, attributes, index, **uniforms)
