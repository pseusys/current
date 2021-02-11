from OpenGL.GL import *
from PyQt6.QtOpenGLWidgets import QOpenGLWidget


class GlWidget(QOpenGLWidget):
    def __init__(self, parent=None):
        QOpenGLWidget.__init__(self, parent)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)
        glColor3f(1.0, 1.5, 0.0)
        glBegin(GL_LINES)
        glVertex3f(1.0, 0.0, 0.0)
        glVertex3f(0.0, 1.0, 0.0)
        glVertex3f(0.0, 0.0, 0.0)
        glEnd()

    def initializeGL(self):
        pass

    def set_mode(self, mode):
        pass

    def add_vertex(self, mode):
        pass

    def clear_vertexes(self, mode):
        pass
