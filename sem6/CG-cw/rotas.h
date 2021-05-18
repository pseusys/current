#ifndef ROTAS_H
#define ROTAS_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

class Rotas {
public:
    Rotas();
    void setup(QOpenGLShaderProgram* program, const char* matrixAttrributeName = "pvm_matrix");

    void resize(int w, int h);
    void translate(int x, int y, int z);
    void rotate(int degrees, bool x, bool y, bool z);

//private:
    QMatrix4x4 model, view, projection;
    QMatrix4x4 cameraTransformation;
    QVector3D cameraPoint;
};

#endif // ROTAS_H
