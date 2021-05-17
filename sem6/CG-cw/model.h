#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLFunctions>
#include <QOpenGLContext>


class Model {
public:
    Model();
    void setContext(QOpenGLContext* context);
    void build(QString file);
    void drawPath(int num, GLuint program, const char* coordAttrributeName = "coord", const char* colorAttrributeName = "color");

private:
    QPair<QList<GLfloat>, QList<GLfloat>> parsePath(QString* path);

    QOpenGLContext* ctx;
    QList<QPair<QList<GLfloat>, QList<GLfloat>>> paths;
};

#endif // MODEL_H
