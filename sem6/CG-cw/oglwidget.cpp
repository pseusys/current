#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "oglwidget.h"
#include "model.h"

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

void OGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    speaker.build("speaker", "general", "textured");
    speaker.setSquareTexture(22, "img_texture.jpg");

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
    for (int i = 0; i < toDraw; i++) {
        QOpenGLShaderProgram* shader = speaker.getShader(i);
        shader->bind();
        opera.setup(shader);
        zap.light(shader);
        zap.color(shader);
        speaker.draw(i, shader);
        shader->release();
    }
    int drawn = speaker.finish();
    if (drawn == 0) qDebug() << "Alert! No paths redrawn!";
}
