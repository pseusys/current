import sys
from enum import Enum
from OpenGL.GL import *
from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QCheckBox, QVBoxLayout, QLabel, QSlider, QComboBox

from GLTest import GLTest, Func, SFactor, DFactor


class Mode(Enum):
    POINTS = GL_POINTS
    LINES = GL_LINES
    LINE_STRIP = GL_LINE_STRIP
    LINE_LOOP = GL_LINE_LOOP
    TRIANGLES = GL_TRIANGLES
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP
    TRIANGLE_FAN = GL_TRIANGLE_FAN
    QUADS = GL_QUADS
    QUAD_STRIP = GL_QUAD_STRIP
    POLYGON = GL_POLYGON


def configure_test(display, win, test_name):
    check = win.findChild(QCheckBox, test_name + 'Test')
    layout = win.findChild(QVBoxLayout, test_name + 'Args')
    children = [layout.itemAt(x).widget() for x in range(0, layout.count())]

    def toggle_test(checked):
        if test_name == 'scissor':
            test = GLTest.SCISSOR
        elif test_name == 'alpha':
            test = GLTest.ALPHA_FUNK
        elif test_name == 'blend':
            test = GLTest.BLEND_FUNK
        else:
            return
        display.toggle_test(test, checked)
        for c in children:
            c.setEnabled(checked)

    check.stateChanged.connect(toggle_test)

    def set_arg(arg_name, arg_val):
        GLTest.set_arg({arg_name: arg_val})
        display.update()

    for child in children:
        if isinstance(child, QLabel):
            continue
        elif isinstance(child, QSlider):
            child.valueChanged.connect(lambda value, c=child: set_arg(c.objectName(), value / 100))
            child.setRange(0, 100)
            child.setValue(0)
            set_arg(child.objectName(), 0)
        elif isinstance(child, QComboBox):
            if child.objectName() == 'func':
                enumerator = Func
            elif child.objectName() == 'sfactor':
                enumerator = SFactor
            elif child.objectName() == 'dfactor':
                enumerator = DFactor
            else:
                continue
            child.currentIndexChanged.connect(lambda index, c=child: set_arg(c.objectName(), c.itemData(index)))
            for enum in enumerator:
                child.addItem(enum.name, enum.value)


def configure_window(win):
    mode_box = win.modeBox
    display = win.mainGLWidget
    reset = win.resetButton

    mode_box.currentIndexChanged.connect(lambda index: display.set_mode(mode_box.itemData(index)))
    for mode in Mode:
        mode_box.addItem(mode.name, mode.value)
    reset.clicked.connect(lambda: display.clear_vertexes())

    configure_test(display, win, 'scissor')
    configure_test(display, win, 'alpha')
    configure_test(display, win, 'blend')


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = uic.loadUi('main.ui')
    configure_window(window)
    window.show()
    sys.exit(app.exec())
