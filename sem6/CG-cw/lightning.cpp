#include "lightning.h"

Lightning::Lightning() {
    lightPos = QVector3D(0.0, 5.0, 3.0);
    lightColor = QVector3D(1.0, 1.0, 1.0);
    lightStrength = 0.3;
}

void Lightning::light(QOpenGLShaderProgram& program, const char* posAN, const char* colorAN, const char* strengthAN) {
    program.setUniformValue(posAN, lightPos);
    program.setUniformValue(colorAN, lightColor);
    program.setUniformValue(strengthAN, lightStrength);
}

void Lightning::translateBy(float x, float y, float z) {
    lightPos += QVector3D(x, y, z);
}
