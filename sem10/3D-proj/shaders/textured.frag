#version 330 core

// size of one texture image
uniform vec2 resolution;

// texture image
uniform sampler2D surface;

// the computed color replaces k_a (true) or k_d (false)
uniform bool glowing;

// fragment position and normal of the fragment
in vec3 w_position, w_normal, w_coordinate;

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
    vec4 col = texture(surface, w_coordinate.xz).rgba;

    vec3 normal_normal = normalize(w_normal);
    vec3 normal_light = normalize(light_pos);
    vec3 normal_view = normalize(w_camera_position - w_position);

    vec3 ambient = glowing ? col.rgb : k_a;
    vec3 material = (glowing ? k_d : col.rgb) * max(dot(normal_normal, normal_light), 0);
    vec3 reflection = k_s * pow(max(dot(reflect(normal_light, normal_normal), -normal_view), 0), 16.) * s;

    out_color = vec4(ambient + material + reflection, a * col.a);
}
