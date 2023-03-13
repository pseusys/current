# Python built-in modules
from bisect import bisect_left      # search sorted keyframe lists

# External, non built-in modules
import OpenGL.GL as GL              # standard Python OpenGL wrapper
import glfw                         # lean window system wrapper for OpenGL

from core import Node
from transform import lerp, quaternion_slerp, quaternion_matrix, translate, scale, identity


# -------------- Keyframing Utilities TP6 ------------------------------------
class KeyFrames:
    """ Stores keyframe pairs for any value type with interpolation_function"""
    def __init__(self, time_value_pairs, interpolation_function=lerp):
        if isinstance(time_value_pairs, dict):  # convert to list of pairs
            time_value_pairs = time_value_pairs.items()
        keyframes = sorted(((key[0], key[1]) for key in time_value_pairs))
        self.times, self.values = zip(*keyframes)  # pairs list -> 2 lists
        self.interpolate = interpolation_function

    def value(self, time):
        """ Computes interpolated value from keyframes, for a given time """

        # 1. ensure time is within bounds else return boundary keyframe
        if time < self.times[0]:
            return self.values[0]
        if time > self.times[-1]:
            return self.values[-1]

        # 2. search for closest index entry in self.times, using bisect_left
        index = bisect_left(self.times, time)
        fraction = (time - self.times[index-1]) / (self.times[index] - self.times[index-1])

        # 3. using the retrieved index, interpolate between the two neighboring
        # values in self.values, using the stored self.interpolate function
        return self.interpolate(self.values[index-1], self.values[index], fraction)


class TransformKeyFrames:
    """ KeyFrames-like object dedicated to 3D transforms """
    def __init__(self, translate_keys, rotate_keys, scale_keys):
        """ stores 3 keyframe sets for translation, rotation, scale """
        self.translate_keyframe = KeyFrames(translate_keys)
        self.rotate_keyframe = KeyFrames(rotate_keys, interpolation_function=quaternion_slerp)
        self.scale_keyframe = KeyFrames(scale_keys)

    def value(self, time):
        """ Compute each component's interpolation and compose TRS matrix """
        translate_int = translate(self.translate_keyframe.value(time))
        rotate_int = quaternion_matrix(self.rotate_keyframe.value(time))
        scale_int = scale(self.scale_keyframe.value(time))
        return translate_int @ rotate_int @ scale_int


class KeyFrameControlNode(Node):
    """ Place node with transform keys above a controlled subtree """
    def __init__(self, trans_keys, rot_keys, scale_keys, transform=identity()):
        super().__init__(transform=transform)
        self.keyframes = TransformKeyFrames(trans_keys, rot_keys, scale_keys)

    def draw(self, primitives=GL.GL_TRIANGLES, **uniforms):
        """ When redraw requested, interpolate our node transform from keys """
        self.transform = self.keyframes.value(glfw.get_time())
        print(self.transform)
        super().draw(primitives=primitives, **uniforms)
