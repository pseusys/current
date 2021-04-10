attribute vec3 coord3d;
varying vec3 vColor;

void main(void) {
    gl_Position = vec4(coord3d.x, coord3d.y, coord3d.z, 1);
    vColor.r = 0.0;
    vColor.g = 0.5;
    vColor.b = 0.5;
}
