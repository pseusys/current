#version 450 core
in vec3 vert_coord;
in vec3 vert_normal;
uniform float vert_shininess;
uniform float vert_specular;
in vec2 vert_tex_coord;

out vec3 frag_coord;
out vec3 frag_normal;
out float frag_specular;
out float frag_shininess;
out vec2 frag_tex_coord;

uniform mat4 projection, view, model;

void main(void) {
    gl_Position = projection * view * model * vec4(vert_coord, 1.0);

    frag_coord = vec3(model * vec4(vert_coord, 1.0));
    frag_normal = mat3(transpose(inverse(model))) * vert_normal;

    frag_tex_coord = vert_tex_coord;
    frag_specular = vert_specular;
    frag_shininess = vert_shininess;

    frag_tex_coord = vert_tex_coord;
}
