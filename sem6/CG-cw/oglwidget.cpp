#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "oglwidget.h"
#include "model.h"

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

void OGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/general.frag");
    shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/general.vert");
    shader.link();

    Material main = { QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0), 0.3, 4, 0.5 };
    Material crown = { QVector3D(0.8, 0.8, 0.8), QVector3D(0.8, 0.8, 0.8), 0.0, 1, 0.5 };
    Material tongue = { QVector3D(0.5, 0.5, 0.5), QVector3D(0.5, 0.5, 0.5), 0.0, 1, 0.5 };
    Material handle = { QVector3D(0.7, 0.7, 0.7), QVector3D(0.7, 0.7, 0.7), 0.0, 1, 0.5 };
    Material screen = { QVector3D(0.5, 0.5, 0.5), QVector3D(1.0, 1.0, 1.0), 0.3, 4, 1.0 };

    speaker.build(":/speaker.mdl");
    for (int i = 0; i < 16; i++) speaker.setMaterial(i, main);
    speaker.setMaterial(16, crown);
    speaker.setMaterial(17, tongue);
    speaker.setMaterial(18, crown);
    speaker.setMaterial(19, main);
    speaker.setMaterial(20, crown);
    speaker.setMaterial(21, main);
    speaker.setMaterial(22, screen);
    speaker.setMaterial(23, main);
    speaker.setMaterial(24, main);
    speaker.setMaterial(25, crown);
    speaker.setMaterial(26, crown);
    speaker.setMaterial(27, handle);
    speaker.setMaterial(28, crown);
    speaker.setSquareTexture(22, ":/texture.png");

    glClearColor(0, 0, 0, 1);
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    opera.resize(w, h);
}

void OGLWidget::paintGL() {
    opera.init();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int toDraw = speaker.start();
    shader.bind();
    for (int i = 0; i < toDraw; i++) {
        opera.setup(shader);
        zap.light(shader);
        speaker.draw(i, shader);
    }
    shader.release();
    int drawn = speaker.finish();
    if (drawn == 0) qDebug() << "Alert! No paths redrawn!";
}
