#ifndef ROTAS_H
#define ROTAS_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

class Rotas {
public:
    Rotas();
    void setup(QOpenGLShaderProgram& program, const char* projectionAN = "projection", const char* viewAN = "view", const char* modelAN = "model", const char* coordAN = "view_coord");

    void resize(float w, float h);
    bool translateBy(float x, float y, float z);
    void rotateBy(int degrees);
    void init();

    QVector3D cameraPoint, upPosition;

private:
    QVector3D uppreciate(float rotSin, float rotCos, QVector3D basisI, QVector3D basisJ, QVector3D basisK);

    QMatrix4x4 model, view, projection;
};

#endif // ROTAS_H
