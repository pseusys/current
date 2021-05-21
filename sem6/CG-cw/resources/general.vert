#version 450 core
in vec3 vert_coord;
in vec3 vert_normal;
uniform vec3 vert_color;
uniform float vert_shininess;
uniform float vert_specular;

out vec3 frag_coord;
out vec3 frag_normal;
out vec3 frag_color;
out float frag_specular;
out float frag_shininess;

uniform mat4 projection, view, model;

void main(void) {
    gl_Position = projection * view * model * vec4(vert_coord, 1.0);

    frag_coord = vec3(model * vec4(vert_coord, 1.0));
    frag_normal = mat3(transpose(inverse(model))) * vert_normal;

    frag_color = vert_color;
    frag_specular = vert_specular;
    frag_shininess = vert_shininess;
}
