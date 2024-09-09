#version 330 core

// input attribute variable, given per vertex
in vec3 position, normal, coordinate;

// global matrix variables
uniform mat4 model, view, projection;

// position and normal for the fragment shader
out vec3 w_position, w_normal, w_coordinate;

void main() {
    w_normal = (model * vec4(normal, 0)).xyz;
    w_position = (model * vec4(position, 0)).xyz;
    w_coordinate = coordinate;

    // tell OpenGL how to transform the vertex to clip coordinates
    gl_Position = projection * view * model * vec4(position, 1);
}
