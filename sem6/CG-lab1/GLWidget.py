from random import random
from OpenGL.GL import *
from PyQt6.QtOpenGLWidgets import QOpenGLWidget


class GlWidget(QOpenGLWidget):
    def __init__(self, parent=None):
        QOpenGLWidget.__init__(self, parent)
        self._vert = [(0.0, 0.0, random()), (0.5, 0.0, random()), (0.0, 0.5, random())]
        self._mode = GL_POINTS

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)
        glBegin(self._mode)
        for vertex in self._vert:
            glColor3f(vertex[0], vertex[1], vertex[2])
            glVertex2f(vertex[0], vertex[1])
        glEnd()

    def initializeGL(self):
        pass

    def mousePressEvent(self, event):
        center_w = self.width() / 2
        center_h = self.height() / 2
        event_x = event.position().x() - center_w
        event_y = event.position().y() - center_h
        self._vert.append((event_x / center_w, -event_y / center_h, random()))
        self.update()

    def set_mode(self, mode):
        self._mode = mode
        self.update()

    def clear_vertexes(self):
        self._vert = []
        self.update()
