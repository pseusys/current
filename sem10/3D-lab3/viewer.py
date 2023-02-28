#!/usr/bin/env python3
"""
Python OpenGL practical application.
"""

import sys                          # for system arguments

# External, non built-in modules
import OpenGL.GL as GL              # standard Python OpenGL wrapper
import numpy as np                  # all matrix manipulations & OpenGL args
import glfw                         # lean window system wrapper for OpenGL

from core import Shader, Mesh, Viewer, Node, load
from transform import translate, identity, rotate, scale


class Axis(Mesh):
    """ Axis object useful for debugging coordinate frames """
    def __init__(self, shader):
        pos = ((0, 0, 0), (1, 0, 0), (0, 0, 0), (0, 1, 0), (0, 0, 0), (0, 0, 1))
        col = ((1, 0, 0), (1, 0, 0), (0, 1, 0), (0, 1, 0), (0, 0, 1), (0, 0, 1))
        super().__init__(shader, attributes=dict(position=pos, color=col))

    def draw(self, primitives=GL.GL_LINES, **uniforms):
        super().draw(primitives=primitives, **uniforms)


class Triangle(Mesh):
    """Hello triangle object"""
    def __init__(self, shader):
        position = np.array(((0, .5, 0), (-.5, -.5, 0), (.5, -.5, 0)), 'f')
        color = np.array(((1, 0, 0), (0, 1, 0), (0, 0, 1)), 'f')
        self.color = (1, 1, 0)
        attributes = dict(position=position, color=color)
        super().__init__(shader, attributes=attributes)

    def draw(self, primitives=GL.GL_TRIANGLES, **uniforms):
        super().draw(primitives=primitives, global_color=self.color, **uniforms)

    def key_handler(self, key):
        if key == glfw.KEY_C:
            self.color = (0, 0, 0)


class Cylinder(Node):
    """ Very simple cylinder based on provided load function """
    def __init__(self, shader):
        super().__init__()
        self.add(*load('cylinder.obj', shader))  # just load cylinder from file


# -------------- main program and scene setup --------------------------------
def main():
    """ create a window, add scene objects, then run rendering loop """
    viewer = Viewer()

    # default color shader
    shader = Shader("color.vert", "color.frag")

    # ---- let's make our shapes ----
    # think about it: we can re-use the same cylinder instance!
    cylinder = Cylinder(shader)
    y_rot = (0., 0., 1.)

    # make a flat cylinder
    base_shape = Node(transform=scale(1, 0.5, 1))
    base_shape.add(cylinder)  # shape of robot base

    # make a thin cylinder
    arm_shape = Node(transform=rotate(y_rot, 45) @ translate(0, 2, 0) @ scale(0.5, 2, 0.5))
    arm_shape.add(cylinder)  # shape of arm
    base_shape.add(arm_shape)

    # make a thin cylinder
    forearm_shape = Node(transform=rotate(y_rot, 90) @ translate(1.25, 1, 0) @ scale(0.25, 2, 0.5))
    forearm_shape.add(cylinder)  # shape of forearm
    arm_shape.add(forearm_shape)

    viewer.add(base_shape)

    # start rendering loop
    viewer.run()


if __name__ == '__main__':
    main()                     # main function keeps variables locally scoped
