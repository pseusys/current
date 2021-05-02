#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
//#include <GL/glu.h>
#include "oglwidget.h"

GLfloat PI = 3.14;

OGLWidget::OGLWidget(QWidget *parent): QOpenGLWidget(parent) {}
OGLWidget::~OGLWidget() {}



int curWidth, curHeight;

GLint program;

void OGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    float pos[4] = {0.0, -2.0, -2.0, 1};
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
    glViewport(0, 0, w, h);
    curWidth = w;
    curHeight = h;
}

void OGLWidget::paintGL() {
    float ambient0[4] = {(float) 0.5 * intensity / 100, (float) 0.5 * intensity / 100, (float) 0.5 * intensity / 100, (float) 1};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vert();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (ortog) glOrtho (-1.0, 1.0, -1.0, 1.0, 0.1, 100.0);
    //else gluPerspective (90.0, curWidth / curHeight, 1.0, 100.0);
    //gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    glDisable(GL_LIGHT0);
     glDisable(GL_LIGHT1);
     glDisable(GL_LIGHT2);
     glDisable(GL_LIGHT3);
     glDisable(GL_LIGHT4);
     glDisable(GL_LIGHT5);
     glDisable(GL_LIGHT6);
     glDisable(GL_LIGHT7);

 // свойства материала
     if (material_sample == 1)
     {
         GLfloat material_diffuse[] = {0.8,0.8,0.8,1.0}; // диффузное
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
         GLfloat material_ambient[] = {0.2,0.2,0.2,1.0}; // рассеянный
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
         GLfloat material_specular[] = {0.0,0.0,0.0,1.0}; // зеркальное
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
         GLfloat material_emission[] = {0.0,0.0,0.0,1.0}; // интенсивность
         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
         GLfloat material_shininess[] = {128}; // степень интенсивности
         glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);
     }
     else if (material_sample == 2)
     {
         GLfloat material_diffuse[] = {0.0,0.0,0.0,1.0}; // диффузное
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
         GLfloat material_ambient[] = {0.2,0.2,0.2,1.0}; // рассеянный
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
         GLfloat material_specular[] = {0.0,0.0,0.0,1.0}; // зеркальное
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
         GLfloat material_emission[] = {0.5,0.0,0.0,1.0}; // интенсивность
         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
         GLfloat material_shininess[] = {64}; // степень интенсивности
         glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);
     }
     else if (material_sample == 3)
     {
         GLfloat material_diffuse[] = {0.8,0.8,0.8,1.0}; // диффузное
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
         GLfloat material_ambient[] = {0.0,0.0,0.0,1.0}; // рассеянный
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
         GLfloat material_specular[] = {0.0,0.0,0.0,1.0}; // зеркальное
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
         GLfloat material_emission[] = {0.0,0.5,0.0,1.0}; // интенсивность
         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
         GLfloat material_shininess[] = {32}; // степень интенсивности
         glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);
     }else
     {
         GLfloat material_diffuse[] = {0.8,0.8,0.8,1.0}; // диффузное
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
         GLfloat material_ambient[] = {0.2,0.2,0.2,1.0}; // рассеянный
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
         GLfloat material_specular[] = {0.5,0.5,0.5,1.0}; // зеркальное
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
         GLfloat material_emission[] = {0.0,0.0,0.5,1.0}; // интенсивность
         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
         GLfloat material_shininess[] = {0}; // степень интенсивности
         glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);
     }



    glEnable(GL_LIGHTING);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    // установка источников света
    if (light_sample == 1)
    {
        // направленный источник света
        GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0}; //цвет диффузного освещения
        GLfloat light0_direction[] = {0.0,0.0,0.0,1.0};//положение источника

        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
        glEnable(GL_LIGHT0);
    }
    else if (light_sample == 2)
    {
        // точечный источник света
        // убывание интенсивности с расстоянием
        // отключено (по умолчанию)
        GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light1_position[] = {0.0,0.0,0.0,1.0};

        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
        glEnable(GL_LIGHT1);
    }
    else if (light_sample == 3)
    {
        // точечный источник света
        // убывание интенсивности с расстоянием
        // задано функцией f(d) = 1.0 / (0.4 * d * d + 0.2 * d)
        GLfloat light2_diffuse[] = {0.4, 0.7, 0.2};
        GLfloat light2_position[] = {0.0, 0.0, 1.0, 1.0};

        glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
        glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
        glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.0);
        glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.2);
        glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.4);
        glEnable(GL_LIGHT2);
    }

    else if (light_sample == 4)
    {
        // прожектор
        // убывание интенсивности с расстоянием
        // отключено (по умолчанию)
        // половина угла при вершине 30 градусов
        // направление на центр плоскости
        GLfloat light3_diffuse[] = {0.4, 0.7, 0.2};
        GLfloat light3_position[] = {0.0, 0.0, 1.0, 1.0};
        GLfloat light3_spot_direction[] = {0.0, 0.0, -1.0, 1.0};

        glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
        glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
        glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30);//максимальный угол разброса
        glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light3_spot_direction);//направление света
        glEnable(GL_LIGHT3);
    }

    else if (light_sample == 5)
    {
        // прожектор
        // убывание интенсивности с расстоянием
        // отключено (по умолчанию)
        // половина угла при вершине 30 градусов
        // направление на центр плоскости
        // включен рассчет убывания интенсивности для прожектора
        GLfloat light4_diffuse[] = {0.4, 0.7, 0.2};
        GLfloat light4_position[] = {0.0, 0.0, 1.0, 1.0};
        GLfloat light4_spot_direction[] = {0.0, 0.0, -1.0};

        glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
        glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
        glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 30);
        glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, light4_spot_direction);
        glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 15.0);
        glEnable(GL_LIGHT4);
    }
    else
    {
        // несколько источников света
        GLfloat light5_diffuse[] = {1.0, 0.0, 0.0};
        GLfloat light5_position[] = {3.0,6.0,5.0,1.0};

        glLightfv(GL_LIGHT5, GL_DIFFUSE, light5_diffuse);
        glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
        glLightf(GL_LIGHT5, GL_CONSTANT_ATTENUATION, 0.0);
        glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION, 0.4);
        glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, 0.8);
        glEnable(GL_LIGHT5);
        GLfloat light6_diffuse[] = {0.0, 1.0, 0.0};
        GLfloat light6_position[] = {1.0,6.0,5.0,1.0};

        glLightfv(GL_LIGHT6, GL_DIFFUSE, light6_diffuse);
        glLightfv(GL_LIGHT6, GL_POSITION, light6_position);
        glLightf(GL_LIGHT6, GL_CONSTANT_ATTENUATION, 0.0);
        glLightf(GL_LIGHT6, GL_LINEAR_ATTENUATION, 0.4);
        glLightf(GL_LIGHT6, GL_QUADRATIC_ATTENUATION, 0.8);
        glEnable(GL_LIGHT6);
        GLfloat light7_diffuse[] = {0.0, 0.0, 1.0};
        GLfloat light7_position[] = {3.0,6.0,2.0,1.0};

        glLightfv(GL_LIGHT7, GL_DIFFUSE, light7_diffuse);
        glLightfv(GL_LIGHT7, GL_POSITION, light7_position);
        glLightf(GL_LIGHT7, GL_CONSTANT_ATTENUATION, 0.0);
        glLightf(GL_LIGHT7, GL_LINEAR_ATTENUATION, 0.4);
        glLightf(GL_LIGHT7, GL_QUADRATIC_ATTENUATION, 0.8);
        glEnable(GL_LIGHT7);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotated(angleX, 1, 0, 0);
    glRotated(angleY, 0, 1, 0);
    glRotated(angleZ, 0, 0, 1);
    coords();
    glTranslated(translateX, translateY, translateZ);
    glScaled(scaleX, scaleY, scaleZ);
    plot();


}
