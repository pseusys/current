#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class Model : protected QOpenGLFunctions {
public:
    Model();
    ~Model();
    void setContext(QOpenGLContext* context);
    void build(QString file);
    void draw(QOpenGLShaderProgram* program, int mode = GL_TRIANGLE_STRIP, const char* coordAttrributeName = "coord", const char* colorAttrributeName = "color");

private:
    QOpenGLBuffer parsePath(QString* path);

    QOpenGLContext* ctx;
    QList<QOpenGLBuffer> VBOs;
};

#endif // MODEL_H
