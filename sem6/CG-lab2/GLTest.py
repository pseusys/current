import enum
from OpenGL.GL import *


class GLTest(enum.Enum):
    def _apply_scissor(self):
        side_x = int(self.args['size_x'] * self.args['rad_x'])
        side_y = int(self.args['size_y'] * self.args['rad_y'])
        offset_x = int((self.args['size_x'] - side_x) / 2)
        offset_y = int((self.args['size_y'] - side_y) / 2)
        print(offset_x, offset_y, side_x, side_y)
        glScissor(offset_x, offset_y, side_x, side_y)

    def _apply_alpha(self):
        print('alphing', self.args['func'], self.args['ref'])

    def _apply_blend(self):
        print('blending', self.args['sfactor'], self.args['dfactor'])

    def __init__(self, value):
        self.apply = lambda: None
        self.args = {}
        if value is GL_SCISSOR_TEST:
            self.args = {'rad_x': 0.5, 'rad_y': 0.3, 'size_x': 800, 'size_y': 600}  # 1 1
            self.apply = self._apply_scissor
        elif value is GL_ALPHA_TEST:
            self.args = {'func': GL_ALWAYS, 'ref': 0}
            self.apply = self._apply_alpha
        elif value is GL_BLEND:
            self.args = {'sfactor': GL_SRC_ALPHA, 'dfactor': GL_ONE_MINUS_SRC_ALPHA}
            self.apply = self._apply_alpha

    def reset_test(self, args):
        def exc(*missing):
            raise Exception("Arguments missing:", missing)

        if self is GLTest.SCISSOR:
            if ('rad_x' not in args) or ('rad_x' not in args) or ('size_x' not in args) or ('size_y' not in args):
                exc('rad_x', 'rad_y', 'size_x', 'size_y')
            else:
                self.args = args
        elif self is GLTest.ALPHA_FUNK:
            if ('func' not in args) or ('ref' not in args):
                exc('func', 'ref')
            else:
                self.args = args
        elif self is GLTest.BLEND_FUNK:
            if ('sfactor' not in args) or ('dfactor' not in args):
                exc('sfactor', 'dfactor')
            else:
                self.args = args

    SCISSOR = GL_SCISSOR_TEST
    ALPHA_FUNK = GL_ALPHA_TEST
    BLEND_FUNK = GL_BLEND
