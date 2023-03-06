#version 330 core

// fragment position and normal of the fragment, in WORLD coordinates
in vec3 w_position, w_normal;

// light dir, in world coordinates
uniform vec3 light_dir;

// material properties
uniform vec3 k_a, k_d, k_s;
uniform float s;

// global matrix variables
uniform mat4 model;
uniform mat4 view;

// world camera position
uniform vec3 w_camera_position;

// output fragment color for OpenGL
out vec4 out_color;

uniform sampler2D diffuse_map;
in vec2 frag_tex_coords;

void main() {
    vec3 normal_normal = normalize(w_normal);
    vec3 normal_light = light_dir;
    vec3 normal_view = normalize(w_camera_position - w_position);

    vec3 ambient = k_a;
    vec3 image = texture(diffuse_map, frag_tex_coords).rgb;
    vec3 material = image * max(dot(normal_normal, normal_light), 0);
    vec3 reflect = k_s * pow(max(dot(reflect(normal_light, normal_normal), normal_view), 0), s);

    out_color = vec4(ambient + material + reflect, 1);
}
