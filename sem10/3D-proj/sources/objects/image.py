from typing import Tuple

import OpenGL.GL as GL
import numpy as np

from ..utils import identity
from ..wrapper import Shader, Textured, Node, Mesh, Texture


class Image(Textured, Node):
    def __init__(self, shader: Shader, texture: str, ambient: Tuple[float, float, float], shining: Tuple[float, float, float], shininess: float = .1, alpha: float = 1., xlen: float = 1, zlen: float = .05, glowing: bool = False, transform=identity(), **_):
        Node.__init__(self, tuple(), transform)
        normals = [(0, 1, 0)] * 4
        position = [
            (0, 0, 0),
            (xlen, 0, 0),
            (xlen, 0, zlen),
            (0, 0, zlen)
        ]
        coordinate = [
            (0, 0, 0),
            (0, 0, 1),
            (1, 0, 1),
            (1, 0, 0)
        ]
        index = [
            3, 2, 0,
            2, 1, 0
        ]

        uniforms = dict(k_a=ambient, k_s=shining, s=shininess, a=alpha, glowing=glowing)
        attributes = dict(position=position, normal=normals, coordinate=coordinate)
        mesh = Mesh(shader, attributes=attributes, index=index, **uniforms)
        Textured.__init__(self, mesh, surface=Texture(texture))

    def draw(self, primitives=GL.GL_TRIANGLES, model=identity(), **uniforms):
        Node.draw(self, model=model, **uniforms)
        Textured.draw(self, primitives, model=self.world_transform, **uniforms)
