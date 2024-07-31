#version 330 core

// input attribute variable, given per vertex
in vec3 position;

// global matrix variables
uniform mat4 view, projection;

out vec3 tex_coords;

void main() {
    // initialize interpolated colors at vertices
    tex_coords = position;
    tex_coords[1] += 0.4;

    mat4 pv = projection * view;
    pv[3] = vec4(0, 0, 0, 1);

    // tell OpenGL how to transform the vertex to clip coordinates
    gl_Position = pv * vec4(position, 1);
}
