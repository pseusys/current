attribute vec2 coord2d;
attribute vec3 color;
varying vec3 vColor;

void main(void) {
    gl_Position = vec4(coord2d.x, coord2d.y, 0, 1);
    vColor = color;
}
