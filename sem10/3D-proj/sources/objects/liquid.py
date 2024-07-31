import glfw
import numpy as np
from OpenGL import GL

from ..wrapper import Shader, Mesh, Texture, Textured


class Liquid(Textured):
    def __init__(self, shader: Shader, texture_file: str, normal_file: str, radius: int, height: int, amplitude=.01, center_shift=0, speed=4., distortion=6., transparency=1., shininess=.1, glowing=False):
        positions = [(-radius, height, -radius), (radius, height, -radius), (radius, height, radius), (-radius, height, radius)]
        resolution = np.array([radius, radius], dtype=np.uint32)
        indices = np.array((0, 2, 1, 0, 3, 2), dtype=np.uint32)

        colors = dict(k_a=(0, 0, 0), k_d=(0, 0, 0), k_s=(1, 1, 1), s=shininess)
        uniforms = dict(resolution=resolution, amplitude=amplitude, center_shift=center_shift, speed=speed, distortion=distortion, transparency=transparency, glowing=glowing)
        mesh = Mesh(shader, attributes=dict(position=positions), index=indices, **uniforms, **colors)

        super().__init__(mesh, surface=Texture(texture_file), normal=Texture(normal_file))

    def draw(self, primitives=GL.GL_TRIANGLES, **uniforms):
        super().draw(primitives, **uniforms, time=glfw.get_time())
