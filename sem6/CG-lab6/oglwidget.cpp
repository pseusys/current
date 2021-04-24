#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glu.h>
#include "oglwidget.h"

OGLWidget::OGLWidget(QWidget *parent): QOpenGLWidget(parent) {}
OGLWidget::~OGLWidget() {}



GLint program;

void OGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    float pos[4] = {0.0, 0.0, -2.0, 1};
    float dir[3] = {-1, -1, -1};
    float ambient[4] = {0.3, 0.3, 0.3, 1};
    float ambient0[4] = {0.5, 0.5, 0.5, 1};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);

    GLfloat front_color[] = {0.7, 0, 0, 1};
    GLfloat back_color[] = {0, 0, 0.7, 1};

    GLfloat front_ambient[] = {0.5, 0, 0, 1};
    GLfloat back_ambient[] = {0, 0, 0.5, 1};

    GLfloat mat_specular[] = {0.3, 0.3, 0.3, 1};

    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_color);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_color);

    glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
    glMaterialfv(GL_BACK, GL_AMBIENT, back_ambient);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular);

    glMaterialf(GL_FRONT, GL_SHININESS, 0.1);
    glMaterialf(GL_BACK, GL_SHININESS, 0.1);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glClearColor(0.1f, 0.15f, 0.05f, 1.0f);
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(ortog){
        glOrtho(-1.0f,1.0f,-1.0f,1.0f,0.1f,100.0f);
    }else if(persp){
        gluPerspective(90.0, w / h, 1.0, 100.0);
    }

    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float ambient0[4] = {(float) 0.5 * intensity / 100, (float) 0.5 * intensity / 100, (float) 0.5 * intensity / 100, (float) 1};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);

    vert();
    glLoadIdentity();
    glRotated(angleX, 1, 0, 0);
    glRotated(angleY, 0, 1, 0);
    glRotated(angleZ, 0, 0, 1);
    coords();
    glTranslated(translateX, translateY, translateZ);
    glScaled(scaleX, scaleY, scaleZ);
    plot();
}
