#include <QFile>
#include <QRegularExpression>

#include "model.h"

Model::Model() {}

Model::~Model() {
    foreach (QOpenGLBuffer VBO, VBOs) {
        VBO.destroy();
    }
}

void Model::setContext(QOpenGLContext* context) {
    ctx = context;
}

void Model::build(QString file) {
    QString prefix(":/");
    QString code;

    QFile source(prefix + file);
    if (!source.open(QIODevice::ReadOnly)) qDebug() << "Model read error, file: " << file;
    else code = source.readAll();
    source.close();

    QList paths_data = code.split("\r\n\r\n", Qt::SkipEmptyParts);
    foreach (QString path, paths_data) {
        if (path.startsWith("//")) continue;
        VBOs.append(parsePath(&path));
    }
}

QOpenGLBuffer Model::parsePath(QString* path) {
    QString codes_path, colors_path;
    QList splittedPath = path->split("\r\n", Qt::SkipEmptyParts).filter(QRegularExpression("((?:(?:[+-]?[0-9]+.[0-9]+)[, ]*)+)"));
    codes_path = splittedPath[0];
    colors_path = splittedPath[1];

    QRegularExpression splitter("( |,)");
    QList codesStr = codes_path.split(splitter, Qt::SkipEmptyParts);
    QList codes = QList<GLfloat>();

    foreach (QString code, codesStr) {
        codes.append(code.toFloat());
    }

    QList colorsStr = colors_path.split(splitter, Qt::SkipEmptyParts);
    QList colors = QList<GLfloat>();
    foreach (QString color, colorsStr) {
        colors.append(color.toFloat());
    }

    int codes_len = codes.length() * sizeof(GLfloat);
    int colors_len = colors.length() * sizeof(GLfloat);
    if (codes_len != colors_len) throw std::runtime_error("Model error, one of the paths vertexes and colors lengths do not match.");

    QOpenGLBuffer VBO(QOpenGLBuffer::VertexBuffer);
    VBO.create();
    VBO.bind();
    VBO.allocate(codes_len + colors_len);
    VBO.write(0, codes.constBegin(), codes_len);
    VBO.write(codes_len, colors.constBegin(), colors_len);
    VBO.release();

    return VBO;
}


void Model::draw(QOpenGLShaderProgram* program, int mode, const char* coordAttrributeName, const char* colorAttrributeName) {
    foreach (QOpenGLBuffer VBO, VBOs) {
        VBO.bind();
        program->enableAttributeArray(coordAttrributeName);
        program->setAttributeBuffer(coordAttrributeName, GL_FLOAT, 0, 3);
        program->enableAttributeArray(colorAttrributeName);
        program->setAttributeBuffer(colorAttrributeName, GL_FLOAT, VBO.size() / 2, 3);

        ctx->functions()->glDrawArrays(mode, 0, VBO.size() / 6 / sizeof(GLfloat));

        program->disableAttributeArray(coordAttrributeName);
        program->disableAttributeArray(colorAttrributeName);
        VBO.release();
    }
}
