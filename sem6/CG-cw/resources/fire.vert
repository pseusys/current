#version 450 core
attribute vec3 coord;
attribute vec3 color;

varying vec3 vColor;

uniform mat4 pvm_matrix;

void main(void) {
    gl_Position = pvm_matrix * vec4(coord.x, coord.y, coord.z, 1);
    vColor = color;
}
