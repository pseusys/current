#include <QVector3D>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    updateCamera();
    updateLight();
}

MainWindow::~MainWindow() {
    delete ui;
}



void MainWindow::updateCamera() {
    const QVector3D CP = ui->canvas->opera.cameraPoint, UP = ui->canvas->opera.upPosition;
    ui->camera_pos_label->setText("Camera x: " + qFuzzy(CP.x()) + "; y: " + qFuzzy(CP.y()) + "; z: " + qFuzzy(CP.z()));
    ui->camera_dir_label->setText("Angle x: " + qFuzzy(UP.x()) + "; y: " + qFuzzy(UP.y()) + "; z: " + qFuzzy(UP.z()));
}

void MainWindow::updateLight() {
    const QVector3D LP = ui->canvas->zap.lightPos;
    ui->light_pos_label->setText("Light x: " + qFuzzy(LP.x()) + "; y: " + qFuzzy(LP.y()) + "; z: " + qFuzzy(LP.z()));
}

QString MainWindow::qFuzzy(float num) {
    return QString::number(qFuzzyIsNull(num) ? 0 : num, 'f', 2);
}



void MainWindow::on_y_plus_eye_clicked() {
    if (!ui->y_minus_eye->isEnabled()) ui->y_minus_eye->setEnabled(true);
    bool result = ui->canvas->opera.translateBy(0.0, 0.2, 0.0);
    if (!result) ui->y_plus_eye->setEnabled(false);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_y_minus_eye_clicked() {
    if (!ui->y_plus_eye->isEnabled()) ui->y_plus_eye->setEnabled(true);
    bool result = ui->canvas->opera.translateBy(0.0, -0.2, 0.0);
    if (!result) ui->y_minus_eye->setEnabled(false);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_x_plus_eye_clicked() {
    if (!ui->x_minus_eye->isEnabled()) ui->x_minus_eye->setEnabled(true);
    bool result = ui->canvas->opera.translateBy(0.2, 0.0, 0.0);
    if (!result) ui->x_plus_eye->setEnabled(false);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_x_minus_eye_clicked() {
    if (!ui->x_plus_eye->isEnabled()) ui->x_plus_eye->setEnabled(true);
    bool result = ui->canvas->opera.translateBy(-0.2, 0.0, 0.0);
    if (!result) ui->x_minus_eye->setEnabled(false);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_z_plus_eye_clicked() {
    if (!ui->z_minus_eye->isEnabled()) ui->z_minus_eye->setEnabled(true);
    bool result = ui->canvas->opera.translateBy(0.0, 0.0, 0.2);
    if (!result) ui->z_plus_eye->setEnabled(false);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_z_minus_eye_clicked() {
    if (!ui->z_plus_eye->isEnabled()) ui->z_plus_eye->setEnabled(true);
    bool result = ui->canvas->opera.translateBy(0.0, 0.0, -0.2);
    if (!result) ui->z_minus_eye->setEnabled(false);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_rot_right_eye_clicked() {
    ui->canvas->opera.rotateBy(-5);
    ui->canvas->update();
    updateCamera();
}

void MainWindow::on_rot_left_eye_clicked() {
    ui->canvas->opera.rotateBy(5);
    ui->canvas->update();
    updateCamera();
}



void MainWindow::on_y_plus_light_clicked() {
    ui->canvas->zap.translateBy(0.0, 1.0, 0.0);
    ui->canvas->update();
    updateLight();
}

void MainWindow::on_y_minus_light_clicked() {
    ui->canvas->zap.translateBy(0.0, -1.0, 0.0);
    ui->canvas->update();
    updateLight();
}

void MainWindow::on_x_plus_light_clicked() {
    ui->canvas->zap.translateBy(1.0, 0.0, 0.0);
    ui->canvas->update();
    updateLight();
}

void MainWindow::on_x_minus_light_clicked() {
    ui->canvas->zap.translateBy(-1.0, 0.0, 0.0);
    ui->canvas->update();
    updateLight();
}

void MainWindow::on_z_plus_light_clicked() {
    ui->canvas->zap.translateBy(0.0, 0.0, 1.0);
    ui->canvas->update();
    updateLight();
}

void MainWindow::on_z_minus_light_clicked() {
    ui->canvas->zap.translateBy(0.0, 0.0, -1.0);
    ui->canvas->update();
    updateLight();
}
