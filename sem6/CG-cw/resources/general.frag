#version 450 core
uniform vec3 light_coord;
uniform vec3 light_color;
uniform float light_strength;
uniform vec3 view_coord;

in vec3 frag_coord;
in vec3 frag_normal;
in vec3 frag_color;
in float frag_specular;
in float frag_shininess;

out vec4 FragColor;

void main(void) {
    float ambient_strength = 0.5;
    vec3 ambient = vec3(1.0, 1.0, 1.0) * ambient_strength;

    vec3 light_dir = normalize(light_coord - frag_coord);
    float diff = max(dot(frag_normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color * light_strength;

    vec3 view_dir = normalize(view_coord - frag_coord);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), frag_shininess);
    vec3 specular = frag_specular * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * frag_color;
    FragColor = vec4(result, 1.0);
}
