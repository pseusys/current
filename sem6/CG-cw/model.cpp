#include <QFile>
#include <QRegularExpression>

#include "model.h"

Model::Model() {
    textures = QHash<int, QOpenGLTexture*>();
}

Model::~Model() {
    foreach (QOpenGLBuffer VBO, VBOs) {
        VBO.destroy();
    }
    foreach (QOpenGLTexture* texture, textures.values()) {
        delete texture;
    }
}

const GLfloat texCoords [8] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};



void Model::build(QString file) {
    QString raw;
    QFile source(file);
    if (!source.open(QIODevice::ReadOnly)) qDebug() << "Model read error, file:" << source.fileName();
    else raw = source.readAll();
    source.close();

    QHash<QString, QVector3D> normals;
    QList paths = QList<QList<QPair<QString, QVector3D>>>();

    foreach (QString path, raw.split("\r\n", Qt::SkipEmptyParts).filter(QRegularExpression("((?:(?:[+-]?[0-9]+.[0-9]+)[, ]*)+)"))) {
        QList strings = path.split(" ", Qt::SkipEmptyParts);
        QList vertexes = QList<QPair<QString, QVector3D>>();

        for (int i = 0; i < strings.length(); i++) {
            if (!normals.contains(strings[i])) normals.insert(strings[i], QVector3D());
            QList coords = strings[i].split(",");
            Q_ASSERT_X(coords.length() == 3, "Model::build(QString)", "A vertex has more or less than 3 coords.");

            vertexes.append(QPair<QString, QVector3D>(strings[i], QVector3D(coords[0].toFloat(), coords[1].toFloat(), coords[2].toFloat())));
            if (i < 2) continue;

            QVector3D cross(QVector3D::crossProduct(vertexes[i - 2].second - vertexes[i].second, vertexes[i - 1].second - vertexes[i].second));
            for (int j = 0; j < 3; j++) normals[strings[i - j]] += cross;
        }

        paths.append(vertexes);
    }

    for (int i = 0; i < paths.length(); i++) {
        int length = paths[i].length() * 3 * sizeof(GLfloat);
        int offset = 0;

        QOpenGLBuffer VBO(QOpenGLBuffer::VertexBuffer);
        VBO.create();
        VBO.bind();
        VBO.allocate(length * 2 + sizeof(texCoords));

        for (int j = 0; j < paths[i].length(); j++) {
            GLfloat vert [3] = {paths[i][j].second.x(), paths[i][j].second.y(), paths[i][j].second.z()};
            int len = sizeof(vert);
            VBO.write(offset, vert, len);
            offset += len;
        }
        for (int j = 0; j < paths[i].length(); j++) {
            if (!normals.contains(paths[i][j].first)) qDebug() << "A vertex does not have normal.";
            QVector3D normal = normals[paths[i][j].first].normalized();
            GLfloat norm [3] = {normal.x(), normal.y(), normal.z()};
            int len = sizeof(norm);
            VBO.write(offset, norm, len);
            offset += len;
        }

        VBO.release();
        VBOs.append(VBO);
    }
}

void Model::setSquareTexture(int path, QString file) {
    Q_ASSERT_X((path > 0) && (path < VBOs.length()), "Model::setTexture(int, QString)", "Invalid path number.");

    VBOs[path].bind();
    Q_ASSERT_X(VBOs[path].size() == sizeof(GLfloat) * 32, "Model::setTexture(int, QString)", "Path is not applicable for square texture.");

    QOpenGLTexture* texture = new QOpenGLTexture(QImage(file));
    textures.insert(path, texture);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->create();

    VBOs[path].write(VBOs[path].size() - sizeof(texCoords), texCoords, sizeof(texCoords));
    VBOs[path].release();
}



int Model::start() {
    Q_ASSERT_X(!drawing, "Model::start()", "Drawing already in progress.");
    drawing = true;
    drawn = 0;
    glEnable(GL_DEPTH_TEST);
    return VBOs.length();
}

void Model::draw(int path, QOpenGLShaderProgram& program, int mode, const char* coordAN, const char* normalAN, const char* texturedAN, const char* texCoordAN, const char* textureAN) {
    Q_ASSERT_X(drawing, "Model::draw(int, QOpenGLShaderProgram&, int, const char*, const char*)", "Drawing not in progress.");
    drawn++;
    QOpenGLBuffer VBO = VBOs[path];
    bool drawTexture = textures.contains(path);

    VBO.bind();
    int size = VBO.size() - sizeof(texCoords);

    program.enableAttributeArray(coordAN);
    program.setAttributeBuffer(coordAN, GL_FLOAT, 0, 3);

    program.enableAttributeArray(normalAN);
    program.setAttributeBuffer(normalAN, GL_FLOAT, size / 2, 3);

    if (drawTexture) {
        program.enableAttributeArray(texCoordAN);
        program.setAttributeBuffer(texCoordAN, GL_FLOAT, size, 2);

        textures[path]->bind(0);
        program.setUniformValue(textureAN, 0);

        program.setUniformValue(texturedAN, 1);
    } else program.setUniformValue(texturedAN, 0);

    glDrawArrays(mode, 0, size / 6 / sizeof(GLfloat));

    if (drawTexture) {
        program.disableAttributeArray(texCoordAN);
        textures[path]->release(0);
    }

    program.disableAttributeArray(coordAN);
    program.disableAttributeArray(normalAN);
    VBO.release();
}

void Model::color(int path, QOpenGLShaderProgram& program, const char* colorAN, const char* specularAN, const char* shininessAN) {
    QVector3D materialColor = QVector3D(1.0, 1.0, 1.0);
    GLfloat materialSpecular = 0.3;
    GLfloat materialShininess = 4;
    program.setUniformValue(colorAN, materialColor);
    program.setUniformValue(specularAN, materialSpecular);
    program.setUniformValue(shininessAN, materialShininess);
}

int Model::finish() {
    Q_ASSERT_X(drawing, "Model::finish()", "Drawing hasn't started.");
    drawing = false;
    glDisable(GL_DEPTH_TEST);
    return drawn;
}
