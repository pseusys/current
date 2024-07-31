from math import cos, pi

import numpy as np
import OpenGL.GL as GL

from sources.time import Chronograph
from ..wrapper import TexturedCubeMap, TextureCubeMap, Shader, Mesh


class SkyBox(TexturedCubeMap):
    def __init__(self, size, shader: Shader, day_texture: str, day_extension: str, night_texture: str, night_extension: str, chronograph: Chronograph):
        self.chrono = chronograph
        front = ((-1, 1, -1), 
                 (1, 1, -1), 
                 (1, 1, 1), 
                 (1, 1, 1), 
                 (-1, 1, 1), 
                 (-1, 1, -1))
        back = ((-1, -1, -1), 
                (-1, -1, 1), 
                (1, -1, -1), 
                (1, -1, -1), 
                (-1, -1, 1), 
                (1, -1, 1))
        left = ((-1, 1, -1), 
                (-1, -1, -1), 
                (1, -1, -1), 
                (1, -1, -1), 
                (1, 1, -1), 
                (-1, 1, -1))
        right = ((-1, -1, 1), 
                 (-1, -1, -1), 
                 (-1, 1, -1), 
                 (-1, 1, -1), 
                 (-1, 1, 1), 
                 (-1, -1, 1))
        top = ((1, -1, -1), 
               (1, -1, 1), 
               (1, 1, 1), 
               (1, 1, 1), 
               (1, 1, -1), 
               (1, -1, -1))
        bottom = ((-1, -1, 1), 
                  (-1, 1, 1), 
                  (1, 1, 1), 
                  (1, 1, 1), 
                  (1, -1, 1), 
                  (-1, -1, 1))

        vertices = front + back + left + right + top + bottom
        scaled = np.array(vertices, np.float32)
        cube = Mesh(shader, attributes=dict(position=scaled))
        day_sky = TextureCubeMap(day_texture, day_extension)
        night_sky = TextureCubeMap(night_texture, night_extension)

        super().__init__(cube, day_sky=day_sky, night_sky=night_sky)

    def draw(self, primitives=GL.GL_TRIANGLES, **uniforms):
        time = (cos(pi * self.chrono.day) + 1) / 2
        super().draw(primitives, time=time, **uniforms)
        
