#version 330 core

// input attribute variable, given per vertex
in vec3 position;

void main() {
    gl_Position = vec4(position, 1);
}
