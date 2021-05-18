#ifndef ROTAS_H
#define ROTAS_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

class Rotas {
public:
    Rotas();
    void setup(QOpenGLShaderProgram* program, const char* matrixAttrributeName = "pvm_matrix");

    void resize(float w, float h);
    void translateBy(float x, float y, float z);
    void rotateBy(int degrees);
    void clearTransform();

private:
    QVector3D uppreciate(float rotSin, float rotCos, QVector3D basisI, QVector3D basisJ, QVector3D basisK, QVector3D up);

    QMatrix4x4 model, view, projection;
    QMatrix4x4 cameraTransformation;
    QVector3D cameraPoint, upPosition;
};

#endif // ROTAS_H
