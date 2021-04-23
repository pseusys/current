#include <iostream>

#include "oglwidget.h"

float f(double x, double z) {
    return abs((x*x + z*z + 1) / sqrt(x*x + z*z)) - 2;
}



void rotate(double rad, std::vector<GLdouble>& point, std::vector<GLdouble>& new_point) {
    double cos_ang = cosf(rad);
    double sin_ang = sinf(rad);

    new_point[0] = cos_ang * point[0] + sin_ang * point[2];
    new_point[1] = point[1];
    new_point[2] = -sin_ang * point[0] + cos_ang * point[2];
}



int slides, rings;
std::vector<std::vector<std::vector<GLdouble>>> verts = {}; // 1: 4, 2; 2: 8, 4; 3: 12, 9; 4: 16, 18;



void OGLWidget::vert() {
    slides = 4 * fineness;
    rings = 18;
    for (int i = 0; i < 4 - fineness; i++) rings /= 2;
    verts = std::vector(slides, std::vector(rings, std::vector(3, 0.0)));

    std::vector<std::vector<GLdouble>> radius = std::vector(rings, std::vector(3, 0.0));
    for (int i = 3; i <= 20; i += (18 / rings)) {
        int num = (i - 3) / (18 / rings);
        if (num >= rings) break;
        radius[num][0] = 0.0;
        radius[num][2] = i / 10.0;
        radius[num][1] = f(radius[num][0], radius[num][2]);
    }

    for (int i = 0; i < slides; i++) {
        double angle = M_PI / (2 * fineness) * i;
        for (int j = 0; j < rings; j++) rotate(angle, radius[j], verts[i][j]);
    }
}

void OGLWidget::plot() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    if (shape_visible) {
        glLineWidth(1.0);
        for (int i = 0; i < slides - 1; i++) {
            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j < rings; j++) {
                glVertex3d(verts[i][j][0], verts[i][j][1], verts[i][j][2]);
                glVertex3d(verts[i + 1][j][0], verts[i + 1][j][1], verts[i + 1][j][2]);
            }
            glEnd();
        }
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i < rings; i++) {
            glVertex3d(verts[slides - 1][i][0], verts[slides - 1][i][1], verts[slides - 1][i][2]);
            glVertex3d(verts[0][i][0], verts[0][i][1], verts[0][i][2]);
        }
        glEnd();
    }

    if (trans_visible) glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


    if (edge_visible) {
        glLineWidth(2.0);
        for (int i = 0; i < slides; i++) {
            glBegin(GL_LINE_STRIP);
            for (int j = 0; j < rings; j++) glVertex3d(verts[i][j][0], verts[i][j][1], verts[i][j][2]);
            glEnd();
        }
        for (int j = 0; j < rings; j++) {
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < slides; i++) glVertex3d(verts[i][j][0], verts[i][j][1], verts[i][j][2]);
            glEnd();
        }
    }

    glDisable(GL_DEPTH_TEST);
}
