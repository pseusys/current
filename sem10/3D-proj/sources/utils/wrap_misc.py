import OpenGL.GL as GL

from .transform import identity
from ..wrapper import Mesh, Node


class MeshedNode(Mesh, Node):
    def __init__(self, shader, children=(), transform=identity(), attributes=None, index=None, usage=GL.GL_STATIC_DRAW, **uniforms):
        Node.__init__(self, children, transform)
        Mesh.__init__(self, shader, attributes, index, usage, **uniforms)

    def draw(self, primitives=GL.GL_TRIANGLES, attributes=None, model=identity(), **uniforms):
        Node.draw(self, model=model, **uniforms)
        Mesh.draw(self, primitives, attributes, model=self.world_transform, **uniforms)
