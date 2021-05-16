#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "oglwidget.h"
#include "shader.h"

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    timer.start(100, this);
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
    GLuint* vboIds = new GLuint[2];
    glGenBuffers(2, vboIds);

    GLint attributeCoord2d = glGetAttribLocation(program, "coord2d");
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeCoord2d,2,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(attributeCoord2d);

    GLint attributeColor = glGetAttribLocation(program, "color");
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeColor,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(attributeColor);

    GLint uniformMilestones = glGetUniformLocation(program, "milestones");
    glUniform2fv(uniformMilestones, 5, (const GLfloat*) milestones);

    GLint uniformRadius = glGetUniformLocation(program, "radius");
    glUniform1i(uniformRadius, radius);

    GLint intensRadius = glGetUniformLocation(program, "intens");
    glUniform1f(intensRadius, intens);

    glDrawArrays(GL_POLYGON, 0, sizeof(vertices) / sizeof(GLfloat));

    glDisableVertexAttribArray(attributeCoord2d);
    glDisableVertexAttribArray(attributeColor);
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
