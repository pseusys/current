#version 450 core
in vec3 vert_coord;
in vec3 vert_normal;
in vec2 vert_tex_coord;
uniform vec3 vert_color;
uniform vec3 vert_ambient;
uniform float vert_shininess;
uniform float vert_specular;
uniform float vert_emission;

out vec3 frag_coord;
out vec3 frag_normal;
out vec3 frag_color;
out vec3 frag_ambient;
out vec2 frag_tex_coord;
out float frag_specular;
out float frag_shininess;
out float frag_emission;

uniform mat4 projection, view, model;

// Основной вершинный шейдер, код отсюда (https://learnopengl.com/Lighting/Basic-lighting).
void main(void) {
    gl_Position = projection * view * model * vec4(vert_coord, 1.0);

    frag_coord = vec3(model * vec4(vert_coord, 1.0));
    frag_normal = mat3(transpose(inverse(model))) * vert_normal;

    frag_color = vert_color;
    frag_specular = vert_specular;
    frag_shininess = vert_shininess;

    frag_tex_coord = vert_tex_coord;

    frag_ambient = vert_ambient;
    frag_emission = vert_emission;
}
