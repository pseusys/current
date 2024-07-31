import glfw                         # lean window system wrapper for OpenGL
import atexit                       # launch a function at exit


# initialize and automatically terminate glfw on exit
glfw.init()
atexit.register(glfw.terminate)

from .shader import Shader  # noqa: E402
from .vertex_array import VertexArray  # noqa: E402
from .mesh import Mesh  # noqa: E402
from .node import Node  # noqa: E402
from .load import load  # noqa: E402
from .viewer import Viewer  # noqa: E402
from .texture import Texture, Textured  # noqa: E402
from .texture_cube_map import TextureCubeMap, TexturedCubeMap  # noqa: E402
