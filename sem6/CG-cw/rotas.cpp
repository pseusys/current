#include "rotas.h"

const QVector3D center(0.0, 0.0, 0.0);

Rotas::Rotas() {
    projection.setToIdentity();
    model.setToIdentity();
    cameraPoint = QVector3D(0.0, 0.0, 5.0);
    upPosition = QVector3D(0.0, 100.0, 0.0);
    view.setToIdentity();
    view.lookAt(cameraPoint, center, upPosition);
}

const float d_near = 0.1, d_far = 100.0, fov = 45.0;

void Rotas::resize(float w, float h) {
    projection.setToIdentity();
    projection.perspective(fov, w / h, d_near, d_far);
}

QVector3D Rotas::uppreciate(float rotSin, float rotCos, QVector3D basisI, QVector3D basisJ, QVector3D basisK, QVector3D up) {
    QMatrix4x4 G(rotCos, -rotSin, 0, 0,
                 rotSin, rotCos, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1);
    QMatrix4x4 Fi(basisJ.x(), basisK.x(), basisI.x(), 0,
                  basisJ.y(), basisK.y(), basisI.y(), 0,
                  basisJ.z(), basisK.z(), basisI.z(), 0,
                  0, 0, 0, 1);
    QMatrix4x4 U = Fi * G * Fi.inverted();
    return U * up;
}

bool Rotas::translateBy(float x, float y, float z) {
    QVector3D newCameraPoint = cameraPoint + QVector3D(x, y, z);
    if (qFuzzyIsNull(newCameraPoint.x()) && qFuzzyIsNull(newCameraPoint.y()) && qFuzzyIsNull(newCameraPoint.z())) return false;

    QVector3D a = cameraPoint.normalized(), b = newCameraPoint.normalized(), c = QVector3D::crossProduct(a, b);
    if (a != b) {
        float prod = QVector3D::dotProduct(a, b);
        QVector3D proj = a, rej = (b - prod * a) / (b - prod * a).length(), cross = c;
        upPosition = uppreciate(c.length(), prod, cross, proj, rej, upPosition);
    }

    cameraPoint = newCameraPoint;
    view.setToIdentity();
    view.lookAt(cameraPoint, center, upPosition);
    return true;
}

void Rotas::rotateBy(int degrees) {
    float rad = qDegreesToRadians(degrees);
    QVector3D i = cameraPoint.normalized(), j = upPosition.normalized(), k = QVector3D::normal(i, j);
    upPosition = uppreciate(sin(rad), cos(rad), i, j, k, upPosition);

    view.setToIdentity();
    view.lookAt(cameraPoint, center, upPosition);
}

void Rotas::setup(QOpenGLShaderProgram& program, const char* projectionAN, const char* viewAN, const char* modelAN, const char* coordAN) {
    program.setUniformValue(projectionAN, projection);
    program.setUniformValue(viewAN, view);
    program.setUniformValue(modelAN, model);
    program.setUniformValue(coordAN, cameraPoint);
}

void Rotas::init() {
    model.setToIdentity();
    model.rotate(270, QVector3D(0, 0, 1));
    model.translate(0, -1.45, 0);
}
