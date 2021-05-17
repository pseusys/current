#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "oglwidget.h"
#include "shader.h"
#include "model.h"

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    timer.start(100, this);
    speaker.build("speaker.mdl");
}

OGLWidget::~OGLWidget() {
    timer.stop();
}



GLfloat vertices[] = {
    -0.8, 0,
    0, 0.8,
    0.8, 0,
    0.2, -0.7,
    0, -0.8,
    -0.2, -0.7
};

GLfloat colors[] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    1, 0.3, 0,
    1, 0.3, 0,
    1, 0.3, 0
};

GLint program;

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    Shader shader(this->context());
    program = shader.compile("fire.vert", "fire.frag");

    speaker.setContext(this->context());

    glClearColor(0,0,0,1);
    initUniforms();
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    GLint uniformMilestones = glGetUniformLocation(program, "milestones");
    glUniform2fv(uniformMilestones, 5, (const GLfloat*) milestones);

    GLint uniformRadius = glGetUniformLocation(program, "radius");
    glUniform1i(uniformRadius, radius);

    GLint intensRadius = glGetUniformLocation(program, "intens");
    glUniform1f(intensRadius, intens);

    speaker.drawPath(0, program);
}

void OGLWidget::timerEvent(QTimerEvent* ev)
{
    if (ev->timerId() != timer.timerId()) {
        QWidget::timerEvent(ev);
        return;
    }
    callBack();
    update();
}
