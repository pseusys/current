#version 330 core

// fragment position and normal of the fragment
in vec3 w_position, w_normal;

// light dir, in world coordinates
uniform vec3 light_pos;

// material properties
uniform vec3 k_a, k_d, k_s;
uniform float s, a;

// world camera position
uniform vec3 w_camera_position;

// output fragment color for OpenGL
out vec4 out_color;

void main() {
    vec3 normal_normal = normalize(w_normal);
    vec3 normal_light = normalize(light_pos);
    vec3 normal_view = normalize(w_camera_position - w_position);

    vec3 ambient = k_a;
    vec3 material = k_d * max(dot(normal_normal, normal_light), 0);
    vec3 reflection = k_s * pow(max(dot(reflect(normal_light, normal_normal), -normal_view), 0), 16.) * s;

    out_color = vec4(ambient + material + reflection, a);
}
