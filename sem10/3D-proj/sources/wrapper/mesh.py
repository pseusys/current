import OpenGL.GL as GL

from .vertex_array import VertexArray


class Mesh:
    """ Basic mesh class, attributes and uniforms passed as arguments """
    def __init__(self, shader, attributes, index=None,
                 usage=GL.GL_STATIC_DRAW, **uniforms):
        self.shader = shader
        self.uniforms = uniforms
        self.vertex_array = VertexArray(shader, attributes, index, usage)

    def draw(self, primitives=GL.GL_TRIANGLES, attributes=None, **uniforms):
        GL.glUseProgram(self.shader.glid)
        self.shader.set_uniforms({**self.uniforms, **uniforms})
        self.vertex_array.execute(primitives, attributes)
