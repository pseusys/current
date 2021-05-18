#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class Model {
public:
    Model();
    ~Model();
    void setContext(QOpenGLContext* context);
    void build(QString file);
    void drawPath(int num, QOpenGLShaderProgram* program, int mode = GL_TRIANGLE_STRIP, const char* coordAttrributeName = "coord", const char* colorAttrributeName = "color");

private:
    QPair<QList<GLfloat>, QList<GLfloat>> parsePath(QString* path);

    QOpenGLContext* ctx;
    QList<QPair<QVector<GLfloat>, QVector<GLfloat>>> paths;

    QOpenGLBuffer verts, cols;
};

#endif // MODEL_H
