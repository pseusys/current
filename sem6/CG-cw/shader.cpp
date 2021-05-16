#include <QFile>

#include "shader.h"

Shader::Shader(QOpenGLContext* context) {
    ctx = context;
}

GLuint Shader::compileShader(QString* filename, GLenum type) {
    QString prefix(":/");
    QString code;

    QFile file(prefix + *filename);
    if (!file.open(QIODevice::ReadOnly)) qDebug() << "Shader read error, file: " << *filename;
    else code = file.readAll();
    file.close();

    const char* codeStr = code.toStdString().c_str();
    GLuint shaderId = ctx->functions()->glCreateShader(type);
    ctx->functions()->glShaderSource(shaderId, 1, &codeStr, NULL);
    ctx->functions()->glCompileShader(shaderId);
    GLint compileOk = GL_FALSE;
    ctx->functions()->glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileOk);
    if (compileOk == GL_FALSE) qDebug() << "Shader compile error, file: " << *filename;

    return shaderId;
}

GLint Shader::compile(QString vert, QString frag) {
    GLuint vertexShaderId = compileShader(&vert, GL_VERTEX_SHADER);
    GLuint fragmentShaderId = compileShader(&frag, GL_FRAGMENT_SHADER);

    GLint program = ctx->functions()->glCreateProgram();
    ctx->functions()->glAttachShader(program, vertexShaderId);
    ctx->functions()->glAttachShader(program, fragmentShaderId);
    ctx->functions()->glLinkProgram(program);
    GLint linkOk = GL_FALSE;
    ctx->functions()->glGetProgramiv(program, GL_LINK_STATUS, &linkOk);
    if (linkOk == GL_FALSE) qDebug() << "Shader link error, files: " << vert << ", " << frag;

    ctx->functions()->glDeleteShader(vertexShaderId);
    ctx->functions()->glDeleteShader(fragmentShaderId);
    return program;
}
