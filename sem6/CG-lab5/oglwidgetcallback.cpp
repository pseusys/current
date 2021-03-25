#include <QRandomGenerator>
#include <iostream>

#include "oglwidget.h"

void OGLWidget::callBack() {
    int min_w = this->width() / 10;
    int max_w = min_w * 9;
    int min_h = this->height() / 10;
    int max_h = min_h * 9;

    for (int i = 0; i < 10; i += 2) {
        int new_x = milestones[i] + QRandomGenerator::global()->bounded(-5, 5);
        int new_y = milestones[i + 1] + QRandomGenerator::global()->bounded(-10, 10);

        if ((new_x <= max_w) && (new_x >= min_w)) milestones[i] = new_x;
        if ((new_y <= max_h) && (new_y >= min_h)) milestones[i + 1] = new_y;
    }

    int is_dim = QRandomGenerator::global()->bounded(-1, 5);
    intens = is_dim < 0 ? 0.75 : 1;

    int ch_rad = QRandomGenerator::global()->bounded(-1, 5);
    int rad = QRandomGenerator::global()->bounded(30, 60);
    radius = ch_rad < 0 ? rad : 45;
}
