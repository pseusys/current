#include <QTransform>

#include "rotas.h"

Rotas::Rotas() {
    projection.setToIdentity();
    model.setToIdentity();
    clearTransform();
}

const float d_near = 0.1, d_far = 100.0, fov = 45.0;

void Rotas::resize(float w, float h) {
    projection.setToIdentity();
    projection.perspective(fov, w / h, d_near, d_far);
}

const QVector3D center(0.0, 1.0, 0.0);

QVector3D Rotas::uppreciate(float rotSin, float rotCos, QVector3D basisI, QVector3D basisJ, QVector3D basisK, QVector3D up) {
    QTransform rotation(rotCos, -rotSin, 0,
                        rotSin, rotCos, 0,
                        0, 0, 1);
    QTransform basis(basisJ.x(), basisK.x(), basisI.x(),
                     basisJ.y(), basisK.y(), basisI.y(),
                     basisJ.z(), basisK.z(), basisI.z());
    QTransform trans = basis * rotation * basis.inverted();
    return QVector3D(trans.m11() * up.x() + trans.m12() * up.y() + trans.m13() * up.z(),
                     trans.m21() * up.x() + trans.m22() * up.y() + trans.m23() * up.z(),
                     trans.m31() * up.x() + trans.m32() * up.y() + trans.m33() * up.z());
}

void Rotas::translateBy(float x, float y, float z) {
    QVector3D newCameraPoint = cameraPoint + QVector3D(x, y, z);

    QVector3D a = cameraPoint.normalized(), b = newCameraPoint.normalized(), c = QVector3D::normal(a, b);
    if (a != b) {
        float prod = QVector3D::dotProduct(a, b);
        QVector3D proj = a, rej = (b - prod * a) / (b - prod * a).length(), cross = QVector3D::crossProduct(b, a);
        upPosition = uppreciate(c.length(), prod, cross, proj, rej, upPosition);
    }

    cameraPoint = newCameraPoint;
    view.setToIdentity();
    view.lookAt(cameraTransformation * cameraPoint, center, upPosition);
}

void Rotas::rotateBy(int degrees) {
    float rad = qDegreesToRadians(degrees);
    QVector3D i = cameraPoint.normalized(), j = upPosition.normalized(), k = QVector3D::normal(i, j);
    upPosition = uppreciate(sin(rad), cos(rad), i, j, k, upPosition);

    view.setToIdentity();
    view.lookAt(cameraTransformation * cameraPoint, center, upPosition);
}

void Rotas::setup(QOpenGLShaderProgram* program, const char* matrixAttrributeName) {
    program->setUniformValue(matrixAttrributeName, projection * view * model);
}

void Rotas::clearTransform() {
    cameraTransformation.setToIdentity();
    cameraPoint = QVector3D(1.0, 0.0, 0.0);
    upPosition = QVector3D(0.0, 1.0, 0.0);
    view.setToIdentity();
    view.lookAt(cameraPoint, center, upPosition);
}
