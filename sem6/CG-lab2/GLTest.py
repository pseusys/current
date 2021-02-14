from enum import Enum
from OpenGL.GL import *


class Func(Enum):
    ALWAYS = GL_ALWAYS
    NEVER = GL_NEVER
    LESS = GL_LESS
    EQUAL = GL_EQUAL
    LEQUAL = GL_LEQUAL
    GREATER = GL_GREATER
    NOTEQUAL = GL_NOTEQUAL
    GEQUAL = GL_GEQUAL


class SFactor(Enum):
    SRC_ALPHA = GL_SRC_ALPHA
    ZERO = GL_ZERO
    ONE = GL_ONE
    DST_COLOR = GL_DST_COLOR
    ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR
    ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA
    DST_ALPHA = GL_DST_ALPHA
    ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA
    SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE


class DFactor(Enum):
    ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA
    ZERO = GL_ZERO
    ONE = GL_ONE
    SRC_COLOR = GL_SRC_COLOR
    ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR
    SRC_ALPHA = GL_SRC_ALPHA
    DST_ALPHA = GL_DST_ALPHA
    ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA


class GLTest(Enum):
    _ignore_ = ['_args']
    _args = {}

    @classmethod
    def _apply_scissor(cls):
        side_x = int(cls._args['size_x'] * (1 - cls._args['rad_x']))
        side_y = int(cls._args['size_y'] * (1 - cls._args['rad_y']))
        offset_x = int((cls._args['size_x'] - side_x) / 2)
        offset_y = int((cls._args['size_y'] - side_y) / 2)
        glScissor(offset_x, offset_y, side_x, side_y)

    @classmethod
    def _apply_alpha(cls):
        glAlphaFunc(cls._args['func'], cls._args['ref'])

    @classmethod
    def _apply_blend(cls):
        glBlendFunc(cls._args['sfactor'], cls._args['dfactor'])

    def apply(self):
        pass

    def __init__(self, value):
        if value is GL_SCISSOR_TEST:
            self.apply = self._apply_scissor
        elif value is GL_ALPHA_TEST:
            self.apply = self._apply_alpha
        elif value is GL_BLEND:
            self.apply = self._apply_blend

    @classmethod
    def set_arg(cls, arg):
        print(arg)
        cls._args.update(arg)

    SCISSOR = GL_SCISSOR_TEST
    ALPHA_FUNK = GL_ALPHA_TEST
    BLEND_FUNK = GL_BLEND


GLTest._args = {
    'rad_x': 0, 'rad_y': 0, 'size_x': 0, 'size_y': 0,
    'func': GL_ALWAYS, 'ref': 0,
    'sfactor': GL_SRC_ALPHA, 'dfactor': GL_ONE_MINUS_SRC_ALPHA
}
