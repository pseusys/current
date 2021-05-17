#include <QFile>
#include <QRegularExpression>

#include "model.h"

Model::Model() {}

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

    QList paths_data = code.split("\r\n\r\n");
    foreach (QString path, paths_data) {
        paths.append(parsePath(&path));
    }
}

QPair<QList<GLfloat>, QList<GLfloat>> Model::parsePath(QString* path) {
    QString codes_path, colors_path;
    QList splittedPath = path->split("\r\n");
    codes_path = splittedPath[0];
    colors_path = splittedPath[1];

    QRegularExpression splitter("(\\ |\\,)");
    QList codesStr = codes_path.split(splitter);
    QList codes = QList<GLfloat>();
    foreach (QString code, codesStr) {
        codes.append(code.toFloat());
        //qDebug() << " Code:" << code.toFloat();
    }

    QList colorsStr = colors_path.split(splitter);
    QList colors = QList<GLfloat>();
    foreach (QString color, colorsStr) {
        colors.append(color.toFloat());
        //qDebug() << " Color:" << color.toFloat();
    }

    return QPair<QList<GLfloat>,QList<GLfloat>>(codes, colors);
}

void Model::drawPath(int num, GLuint program, const char* coordAttrributeName, const char* colorAttrributeName) {
    QPair<QList<GLfloat>, QList<GLfloat>> ptr = paths[num];
    int codes_len = ptr.first.length();
    const GLfloat* codes = ptr.first.toVector().constBegin();
    int colors_len = ptr.second.length();
    const GLfloat* colors = ptr.second.toVector().constBegin();

    GLuint* vboIds = new GLuint[2];
    ctx->functions()->glGenBuffers(2, vboIds);

    GLint attributeCoord2d = ctx->functions()->glGetAttribLocation(program, coordAttrributeName);
    ctx->functions()->glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    ctx->functions()->glBufferData(GL_ARRAY_BUFFER, codes_len * sizeof(GLfloat), codes, GL_STATIC_DRAW);
    ctx->functions()->glVertexAttribPointer(attributeCoord2d,3,GL_FLOAT,GL_FALSE,0,0);
    ctx->functions()->glEnableVertexAttribArray(attributeCoord2d);

    GLint attributeColor = ctx->functions()->glGetAttribLocation(program, colorAttrributeName);
    ctx->functions()->glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    ctx->functions()->glBufferData(GL_ARRAY_BUFFER, colors_len * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    ctx->functions()->glVertexAttribPointer(attributeColor,3,GL_FLOAT,GL_FALSE,0,0);
    ctx->functions()->glEnableVertexAttribArray(attributeColor);

    ctx->functions()->glDrawArrays(GL_POINTS, 0, codes_len);

    ctx->functions()->glDisableVertexAttribArray(attributeCoord2d);
    ctx->functions()->glDisableVertexAttribArray(attributeColor);
}
