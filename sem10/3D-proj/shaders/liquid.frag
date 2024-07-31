#version 330 core

// time elapsed
uniform float time;

// size of one texture image
uniform vec2 resolution;

// texture image
uniform sampler2D surface, normal;

// waves insensitivity
uniform float amplitude;

// animation speed
uniform float speed;

// shift of wave center from the middle
uniform float center_shift;

// wave distortion factor
uniform float distortion;

// liquid transparency
uniform float transparency;

// the computed color replaces k_a (true) or k_d (false)
uniform bool glowing;

// fragment position and normal of the fragment
in vec3 w_position;

// light dir, in world coordinates
uniform vec3 light_pos;

// material properties
uniform vec3 k_a, k_d, k_s;
uniform float s;

// world camera position
uniform vec3 w_camera_position;

// output fragment color for OpenGL
out vec4 out_color;

void main() {
    vec2 center_pos = w_position.xz / resolution.xy - center_shift;
    float center_len = length(center_pos);

    vec2 waving = (center_pos / center_len) * cos(center_len * distortion - time * speed) * amplitude;
    vec2 uv = w_position.xz / resolution.xy + waving;
    vec3 col = texture(surface, uv).xyz;
    vec3 norm = texture(normal, uv).rbg;  

    vec3 normal_normal = normalize(norm * 2.0 - 1.0);
    vec3 normal_light = normalize(light_pos);
    vec3 normal_view = normalize(w_camera_position - w_position);

    vec3 ambient = glowing ? col : k_a;
    vec3 material = (glowing ? k_d : col) * max(dot(normal_normal, normal_light), 0);
    vec3 reflection = k_s * pow(max(dot(reflect(normal_light, normal_normal), -normal_view), 0), 16.) * s;

    out_color = vec4(ambient + material + reflection, transparency);
}
