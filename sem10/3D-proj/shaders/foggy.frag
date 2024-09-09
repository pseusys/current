#version 330 core

// time elapsed
uniform float time;

// fragment position and normal of the fragment
in vec3 w_position;

// light dir, in world coordinates
uniform vec3 light_pos;

// material properties
uniform vec3 k_a, k_d;
uniform float a;

// global matrix variables
uniform mat4 model, view;

// world camera position
uniform vec3 w_camera_position;

// output fragment color for OpenGL
out vec4 out_color;

void main() {
    float light_intense = min(light_pos.y, 0);

    vec3 ambient = k_a;
    vec3 material = k_d * light_intense;

    out_color = vec4(ambient + material, a);
}
