#version 330 core

// input attribute variable, given per vertex
in vec3 position;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// position and normal for the fragment shader, in WORLD coordinates
out vec3 w_position, w_normal;

in vec2 tex_coord;
out vec2 frag_tex_coords;

void main() {
    w_normal = (model * vec4(normal, 0)).xyz;
    w_position = (model * vec4(position, 0)).xyz;

    // tell OpenGL how to transform the vertex to clip coordinates
    gl_Position = projection * view * model * vec4(position, 1);
    frag_tex_coords = tex_coord;
}
