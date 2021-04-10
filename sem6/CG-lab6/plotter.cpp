#include <iostream>

#include "oglwidget.h"

float f(double x, double z) {
    return abs((x*x + z*z + 1) / sqrt(x*x + z*z)) - 2;
}



void rotate(double rad, double* point, double* new_point) {
    double cos_ang = cosf(rad);
    double sin_ang = sinf(rad);

    new_point[0] = cos_ang * point[0] + sin_ang * point[2];
    new_point[1] = point[1];
    new_point[2] = -sin_ang * point[0] + cos_ang * point[2];
}



void OGLWidget::vert() {
    double radius[18][3] = {};
    for (int i = 3; i <= 20; i++) {
        int num = i - 3;
        radius[num][0] = 0.0;
        radius[num][2] = i / 10.0;
        radius[num][1] = f(radius[num][0], radius[num][2]);
        //std::cout << "(" << radius[num][0] << ", " << radius[num][1] << ", " << radius[num][2] << ")" << std::endl;
    }

    for (int i = 0; i < 16; i++) {
        double angle = M_PI / 8.0 * i;
        for (int j = 0; j < 18; j++) rotate(angle, radius[j], verts[i][j]);
    }
}

void OGLWidget::plot() {
    for (int i = 0; i < 16; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < 18; j++) {
            glVertex3dv(verts[i][j]);
            glVertex3dv(verts[i + 1][j]);
        }
        glEnd();
    }
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < 18; i++) {
        glVertex3dv(verts[15][i]);
        glVertex3dv(verts[0][i]);
    }
    glEnd();
}
