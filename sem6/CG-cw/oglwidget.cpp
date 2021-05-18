#include <QOpenGLShaderProgram>
#include <QOpenGLShader>

#include "oglwidget.h"
#include "model.h"

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

QOpenGLShaderProgram program;

void OGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    speaker.build("speaker.mdl");

    program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fire.frag");
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/fire.vert");
    program.link();

    speaker.setContext(this->context());

    glClearColor(0, 0, 0, 1);
    initUniforms();
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    opera.resize(w, h);
}

void OGLWidget::paintGL() {
    opera.clearTransform();
    opera.translateBy(3, 1, 0);
    opera.rotateBy(90);
    //opera.translateBy(-4, 4, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.bind();
    opera.setup(&program);
    speaker.draw(&program);
    program.release();
}
