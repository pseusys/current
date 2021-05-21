#include "lightning.h"

Lightning::Lightning() {
    lightPos = QVector3D(0.0, 5.0, 3.0);
    lightColor = QVector3D(1.0, 1.0, 1.0);
    lightStrength = 0.3;

    materialColor = QVector3D(1.0, 1.0, 1.0);
    materialSpecular = 0.3;
    materialShininess = 4;
}

void Lightning::light(QOpenGLShaderProgram* program, const char* posAN, const char* colorAN, const char* strengthAN) {
    program->setUniformValue(posAN, lightPos);
    program->setUniformValue(colorAN, lightColor);
    program->setUniformValue(strengthAN, lightStrength);
}

void Lightning::color(QOpenGLShaderProgram* program, const char* colorAN, const char* specularAN, const char* shininessAN) {
    program->setUniformValue(colorAN, materialColor);
    program->setUniformValue(specularAN, materialSpecular);
    program->setUniformValue(shininessAN, materialShininess);
}

void Lightning::translateBy(float x, float y, float z) {
    lightPos += QVector3D(x, y, z);
}
