#include "rotas.h"

Rotas::Rotas() {
    model.setToIdentity();
    view.setToIdentity();
    projection.setToIdentity();
}

const float d_near = 0.1, d_far = 100.0, fov = 90.0;

void Rotas::resize(int w, int h) {
    projection.setToIdentity();
    projection.perspective(fov, w / h, d_near, d_far);
}

const QVector3D center(0.0, 1.0, 0.0), up(0.0, 1.0, 0.0);

void Rotas::translate(int x, int y, int z) {
    cameraPoint = QVector3D(x, y, z);
    view.setToIdentity();
    view.lookAt(cameraTransformation * cameraPoint, center, up);
}

void Rotas::rotate(int degrees, bool x, bool y, bool z) {
    cameraTransformation.setToIdentity();
    cameraTransformation.rotate(degrees, x, y, z);
    view.setToIdentity();
    view.lookAt(cameraTransformation * cameraPoint, center, up);
}

void Rotas::setup(QOpenGLShaderProgram* program, const char* matrixAttrributeName) {
    program->setUniformValue(matrixAttrributeName, projection * view * model);
}
