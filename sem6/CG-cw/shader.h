#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions>
#include <QOpenGLContext>

class Shader {
public:
    Shader(QOpenGLContext* context);
    GLint compile(QString vert, QString frag);

private:
    QOpenGLContext* ctx;

    GLuint compileShader(QString* filename, GLenum type);
};

#endif // SHADER_H
