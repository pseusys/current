#version 450 core
uniform vec3 light_coord;
uniform vec3 light_color;
uniform vec3 view_coord;
uniform sampler2D frag_texture;
uniform float light_strength;
uniform int textured;

in vec3 frag_coord;
in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_ambient;
in vec2 frag_tex_coord;
in float frag_specular;
in float frag_shininess;
in float frag_emission;

out vec4 FragColor;

// Основной фрагментный шейдер, код отсюда (https://learnopengl.com/Lighting/Basic-lighting) и немного отсюда (https://learnopengl.com/Getting-started/Textures).
void main(void) {
    vec3 ambient = frag_ambient * frag_emission;

    vec3 light_dir = normalize(light_coord - frag_coord);
    float diff = max(dot(frag_normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color * light_strength;

    vec3 view_dir = normalize(view_coord - frag_coord);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), frag_shininess);
    vec3 specular = frag_specular * spec * light_color;

    vec4 result = vec4((ambient + diffuse + specular) * frag_color, 1.0);
    if (textured == 1) {
        vec4 texture = texture2D(frag_texture, frag_tex_coord);
        vec4 blend = texture + result * (1.0 - texture.w);
        FragColor = blend * vec4(ambient + diffuse + specular, 1.0);
    } else FragColor = result;
}
