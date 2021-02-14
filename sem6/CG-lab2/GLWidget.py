import math
from OpenGL.GL import *
from PyQt6.QtOpenGLWidgets import QOpenGLWidget
from GLTest import GLTest


def _blues(x, y):
    return (2 - abs(x) - abs(y)) / 2


class GlWidget(QOpenGLWidget):
    def __init__(self, parent=None):
        def combinator(grad):
            rad = math.radians(grad)
            mono_x = math.cos(rad) / 2
            mono_y = math.sin(rad) / 2
            return mono_x, mono_y, _blues(mono_x, mono_y)

        QOpenGLWidget.__init__(self, parent)
        self._vert = [combinator(x) for x in range(90, 450, 60)]
        self._mode = GL_POINTS
        self._tests = []

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)
        for test in self._tests:
            glEnable(test.value)
            test.apply()
        glBegin(self._mode)
        for vertex in self._vert:
            glColor3f(abs(vertex[0]), abs(vertex[1]), abs(vertex[2]))
            glVertex2f(vertex[0], vertex[1])
        glEnd()
        for test in self._tests:
            glDisable(test.value)

    def resizeGL(self, w, h):
        GLTest.set_arg({'size_x': w, 'size_y': h})

    def mousePressEvent(self, event):
        center_w = self.width() / 2
        center_h = self.height() / 2
        event_x = event.position().x() - center_w
        event_y = event.position().y() - center_h
        gl_x = event_x / center_w
        gl_y = -event_y / center_h
        self._vert.append((gl_x, gl_y, _blues(gl_x, gl_y)))
        self.update()

    def set_mode(self, mode):
        self._mode = mode
        self.update()

    def clear_vertexes(self):
        self._vert = []
        self.update()

    def toggle_test(self, test, exists):
        if (test in self._tests) and not exists:
            self._tests.remove(test)
        elif (test not in self._tests) and exists:
            self._tests.append(test)
        self.update()
