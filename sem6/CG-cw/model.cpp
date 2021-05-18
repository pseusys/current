#include <QFile>
#include <QRegularExpression>

#include "model.h"

Model::Model() {
    verts.create();
    cols.create();
}

Model::~Model() {
    verts.destroy();
    cols.destroy();
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
        paths.append(parsePath(&path));
    }
}

QPair<QVector<GLfloat>, QVector<GLfloat>> Model::parsePath(QString* path) {
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

    return QPair<QVector<GLfloat>,QVector<GLfloat>>(QVector<GLfloat>(codes), QVector<GLfloat>(colors));
}


void Model::drawPath(int num, QOpenGLShaderProgram* program, int mode, const char* coordAttrributeName, const char* colorAttrributeName) {
    QPair<QVector<GLfloat>, QVector<GLfloat>> ptr = paths[num];
    int codes_len = ptr.first.length();
    const GLfloat* codes = ptr.first.constBegin();
    int colors_len = ptr.second.length();
    const GLfloat* colors = ptr.second.constBegin();

    verts.bind();
    verts.allocate(codes, codes_len);
    cols.bind();
    cols.allocate(colors, colors_len);

    //program->setAttributeBuffer(coordAttrributeName, GL_FLOAT, 0, 3);
    program->enableAttributeArray(coordAttrributeName);
    program->setAttributeArray(coordAttrributeName, codes, 3);
    program->enableAttributeArray(colorAttrributeName);
    program->setAttributeArray(colorAttrributeName, colors, 3);

    ctx->functions()->glDrawArrays(mode, 0, codes_len / 3);

    program->disableAttributeArray(coordAttrributeName);
    program->disableAttributeArray(colorAttrributeName);
}
