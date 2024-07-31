from itertools import cycle         # allows easy circular choice list

import glfw                         # lean window system wrapper for OpenGL
import OpenGL.GL as GL              # standard Python OpenGL wrapper
import numpy as np                  # all matrix manipulations & OpenGL args

from sources.utils import Trackball, identity, rotate, scale, translate

from .node import Node


class Viewer(Node):
    """ GLFW viewer window, with classic initialization & graphics loop """

    def __init__(self, width=640, height=480, distance=10):
        super().__init__()

        # version hints: create GL window with >= OpenGL 3.3 and core profile
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL.GL_TRUE)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
        glfw.window_hint(glfw.SAMPLES, 4)
        glfw.window_hint(glfw.RESIZABLE, True)
        self.win = glfw.create_window(width, height, 'Viewer', None, None)

        # make win's OpenGL context current; no OpenGL calls can happen before
        glfw.make_context_current(self.win)

        # initialize trackball
        self.trackball = Trackball(distance=distance, pitch=25)
        self.mouse = (0, 0)
        # register event handlers
        glfw.set_key_callback(self.win, self.on_key)
        glfw.set_cursor_pos_callback(self.win, self.on_mouse_move)
        glfw.set_scroll_callback(self.win, self.on_scroll)
        glfw.set_window_size_callback(self.win, self.on_size)

        # useful message to check OpenGL renderer characteristics
        print('OpenGL', GL.glGetString(GL.GL_VERSION).decode() + ', GLSL',
              GL.glGetString(GL.GL_SHADING_LANGUAGE_VERSION).decode() +
              ', Renderer', GL.glGetString(GL.GL_RENDERER).decode())

        # initialize GL by setting viewport and default render characteristics
        GL.glClearColor(0.1, 0.1, 0.1, 0.1)
        GL.glEnable(GL.GL_CULL_FACE)   # backface culling enabled (TP2)
        GL.glEnable(GL.GL_DEPTH_TEST)  # depth test now enabled (TP2)

        GL.glEnable(GL.GL_BLEND)
        GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)

        # cyclic iterator to easily toggle polygon rendering modes
        self.fill_modes = cycle([GL.GL_LINE, GL.GL_POINT, GL.GL_FILL])

        self.chronograph = None
        self.splashscreen = None

    def set_time(self, chrono):
        self.chronograph = chrono

    def set_splash(self, splash):
        splash.transform = translate(1, 1, 0) @ scale(2, 2, 1) @ rotate((0, 0, 1), 270) @ rotate((1, 0, 0), 90)
        self.splashscreen = splash
        self.add(splash)
        GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)
        self.draw(view=identity(), projection=identity(), model=identity(), w_camera_position=(0, 1, 0))
        glfw.swap_buffers(self.win)
        glfw.poll_events()

    def run(self):
        """ Main render loop for this OpenGL window """
        if self.splashscreen is not None:
            self.remove(self.splashscreen)

        while not glfw.window_should_close(self.win):
            # clear draw buffer and depth buffer (<-TP2)
            GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)

            win_size = glfw.get_window_size(self.win)
            self.chronograph.update(glfw.get_time())

            # draw our scene objects
            cam_pos = np.linalg.inv(self.trackball.view_matrix())[:, 3]
            self.draw(view=self.trackball.view_matrix(),
                      projection=self.trackball.projection_matrix(win_size),
                      model=identity(),
                      w_camera_position=cam_pos,
                      light_pos=self.chronograph.sun_position)

            # flush render commands, and swap draw buffers
            glfw.swap_buffers(self.win)

            # Poll for and process events
            glfw.poll_events()

    def on_key(self, _win, key, _scancode, action, _mods):
        """ 'Q' or 'Escape' quits """
        view = np.linalg.inv(self.trackball.view_matrix())[:, 3]
        old = (view[0], view[1])

        if action == glfw.PRESS or action == glfw.REPEAT:
            if key == glfw.KEY_ESCAPE or key == glfw.KEY_Q:
                glfw.set_window_should_close(self.win, True)

            if key == glfw.KEY_W or key == glfw.KEY_UP:
                self.trackball.pan(old, (view[0], view[1] - 1))
            if key == glfw.KEY_S or key == glfw.KEY_DOWN:
                self.trackball.pan(old, (view[0], view[1] + 1))
            if key == glfw.KEY_A or key == glfw.KEY_LEFT:
                self.trackball.pan(old, (view[0] + 1, view[1]))
            if key == glfw.KEY_D or key == glfw.KEY_RIGHT:
                self.trackball.pan(old, (view[0] - 1, view[1]))

            if key == glfw.KEY_SPACE:
                glfw.set_time(0.0)

            # call Node.key_handler which calls key_handlers for all drawables
            self.key_handler(key)

    def on_mouse_move(self, win, xpos, ypos):
         """ Rotate on left-click & drag, pan on right-click & drag """
         old = self.mouse
         self.mouse = (xpos, glfw.get_window_size(win)[1] - ypos)
         if glfw.get_mouse_button(win, glfw.MOUSE_BUTTON_LEFT):
             self.trackball.drag(old, self.mouse, glfw.get_window_size(win))
         if glfw.get_mouse_button(win, glfw.MOUSE_BUTTON_RIGHT):
             self.trackball.pan(old, self.mouse)

    def on_scroll(self, win, _deltax, deltay):
        """ Scroll controls the camera distance to trackball center """
        self.trackball.zoom(deltay, glfw.get_window_size(win)[1])

    def on_size(self, _win, _width, _height):
        """ window size update => update viewport to new framebuffer size """
        GL.glViewport(0, 0, *glfw.get_framebuffer_size(self.win))
