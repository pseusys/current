varying vec3 vColor;
uniform vec2 milestones[5];
uniform int radius;
uniform float intens;

float dist(vec2 a, vec2 b) {
    float real = distance(a, b);
    if (real >= radius) return 0.0;
    else return 1 - (real / radius);
}

float min_dist(vec2 position) {
    float less = 0.0;
    for (int i = 0; i < 5; ++i) less = max(less, dist(position, milestones[i]));
    return less;
}

void main(void) {
    float len = min_dist(gl_FragCoord.xy);
    gl_FragColor.r = vColor.r * len * intens;
    gl_FragColor.g = vColor.g * len * intens;
    gl_FragColor.b = vColor.b * len * intens;
}
