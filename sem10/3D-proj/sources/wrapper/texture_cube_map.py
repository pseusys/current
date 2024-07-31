from PIL import Image
import OpenGL.GL as GL


class TextureCubeMap:
    """ Helper class to create and automatically destroy textures """
    def __init__(self, name: str, type: str, tex_type=GL.GL_TEXTURE_CUBE_MAP):
        self.glid = GL.glGenTextures(1)
        self.type = tex_type
        tex_files = ['right', 'left', 'top', 'bottom','front', 'back']

        try:
            for i, ftype in enumerate(tex_files):
                filename = f"{name}_{ftype}.{type}"
                # imports image as a numpy array in exactly right format
                tex = Image.open(filename).convert('RGBA')
                GL.glBindTexture(tex_type, self.glid)
                GL.glTexImage2D(GL.GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
                                GL.GL_SRGB_ALPHA, tex.width, tex.height, 0, GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, tex.tobytes())
                GL.glTexParameteri(tex_type, GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP_TO_EDGE)
                GL.glTexParameteri(tex_type, GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP_TO_EDGE)
                GL.glTexParameteri(tex_type, GL.GL_TEXTURE_WRAP_R, GL.GL_CLAMP_TO_EDGE)
                GL.glTexParameteri(tex_type, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)
                GL.glTexParameteri(tex_type, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)

        except FileNotFoundError:
            print("ERROR: unable to load texture files")

    def __del__(self):  # delete GL texture from GPU when object dies
        GL.glDeleteTextures(self.glid)


# -------------- Textured mesh decorator --------------------------------------
class TexturedCubeMap:
    """ Drawable mesh decorator that activates and binds OpenGL textures """
    def __init__(self, drawable, **textures):
        self.drawable = drawable
        self.textures = textures

    def draw(self, primitives=GL.GL_TRIANGLES, **uniforms):
        GL.glDisable(GL.GL_DEPTH_TEST)
        GL.glDepthMask(GL.GL_FALSE) 

        for index, (name, texture) in enumerate(self.textures.items()):
            GL.glActiveTexture(GL.GL_TEXTURE0 + index)
            GL.glBindTexture(texture.type, texture.glid)
            uniforms[name] = index
        self.drawable.draw(primitives=primitives, **uniforms)

        GL.glDepthMask(GL.GL_TRUE)
        GL.glEnable(GL.GL_DEPTH_TEST)
