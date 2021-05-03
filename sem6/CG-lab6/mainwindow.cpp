#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_up_clicked()
{
    ui->openGLWidget->angleX += 10;
    if(ui->openGLWidget->angleX==360){
        ui->openGLWidget->angleX = 0;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_right_clicked()
{
    ui->openGLWidget->angleY += 10;
    if(ui->openGLWidget->angleY==360){
        ui->openGLWidget->angleY = 0;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_left_clicked()
{
    ui->openGLWidget->angleY -= 10;
    if(ui->openGLWidget->angleY==360){
        ui->openGLWidget->angleY = 0;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_leftZ_clicked()
{
    ui->openGLWidget->angleZ -= 10;
    if(ui->openGLWidget->angleZ==360){
        ui->openGLWidget->angleZ = 0;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_rightZ_clicked()
{
    ui->openGLWidget->angleZ += 10;
    if(ui->openGLWidget->angleZ==360){
        ui->openGLWidget->angleZ = 0;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_down_clicked()
{
    ui->openGLWidget->angleX -= 10;
    if(ui->openGLWidget->angleX==360){
        ui->openGLWidget->angleX = 0;
    }
    ui->openGLWidget->update();
}


void MainWindow::on_scaleXup_clicked()
{
    ui->openGLWidget->scaleX += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_scaleXdown_clicked()
{
    if(ui->openGLWidget->scaleX>=0.1){
        ui->openGLWidget->scaleX -= 0.1;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_scaleYup_clicked()
{
    ui->openGLWidget->scaleY += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_scaleYdown_clicked()
{
    if(ui->openGLWidget->scaleY>=0.1){
        ui->openGLWidget->scaleY -= 0.1;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_scaleZup_clicked()
{
    ui->openGLWidget->scaleZ += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_scaleZdown_clicked()
{
    if(ui->openGLWidget->scaleZ>=0.1){
        ui->openGLWidget->scaleZ -= 0.1;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_translateUp_clicked()
{
    ui->openGLWidget->translateY += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_translateLeft_clicked()
{
    ui->openGLWidget->translateX -= 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_translateRight_clicked()
{
    ui->openGLWidget->translateX += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_translateDown_clicked()
{
    ui->openGLWidget->translateY -= 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_shape_visibility_stateChanged(int arg1)
{
    ui->openGLWidget->shape_visible = arg1;
    ui->openGLWidget->update();
}

void MainWindow::on_edge_visibility_stateChanged(int arg1)
{
    ui->openGLWidget->edge_visible = arg1;
    ui->openGLWidget->update();
}

void MainWindow::on_trans_visibility_stateChanged(int arg1)
{
    ui->openGLWidget->trans_visible = arg1;
    ui->openGLWidget->update();
}

void MainWindow::on_coords_visibility_stateChanged(int arg1)
{
    ui->openGLWidget->coords_visible = arg1;
    ui->openGLWidget->update();
}

void MainWindow::on_fineness_valueChanged(int value)
{
    ui->openGLWidget->fineness = value;
    ui->openGLWidget->update();
}

void MainWindow::on_intensity_valueChanged(int value)
{
    ui->openGLWidget->intensity = value;
    ui->openGLWidget->update();
}

void MainWindow::on_ortho_btn_clicked()
{
    ui->openGLWidget->ortog = 1;
    ui->openGLWidget->update();
}

void MainWindow::on_perspective_btn_clicked()
{
    ui->openGLWidget->ortog = 0;
    ui->openGLWidget->update();
}

void MainWindow::on_up_eye_clicked()
{
    ui->openGLWidget->eyeX++;
    ui->openGLWidget->update();
}

void MainWindow::on_left_eye_clicked()
{
    ui->openGLWidget->eyeY--;
    ui->openGLWidget->update();
}

void MainWindow::on_rightZ_eye_clicked()
{
    ui->openGLWidget->eyeZ++;
    ui->openGLWidget->update();
}

void MainWindow::on_leftZ_eye_clicked()
{
    ui->openGLWidget->eyeZ--;
    ui->openGLWidget->update();
}

void MainWindow::on_right_eye_clicked()
{
    ui->openGLWidget->eyeY++;
    ui->openGLWidget->update();
}

void MainWindow::on_down_eye_clicked()
{
    ui->openGLWidget->eyeX--;
    ui->openGLWidget->update();
}

void MainWindow::on_comboBoxLight_currentTextChanged(const QString &arg1)
{
    if (arg1=="Направленный источник света")
            ui->openGLWidget->light_sample = 1;
    else if(arg1=="Точечный источник света, убывание интенсивности с расстоянием выкл.")
            ui->openGLWidget->light_sample = 2;
    else if(arg1=="Точечный источник света, убывание интенсивности с расстоянием вкл.")
            ui->openGLWidget->light_sample = 3;
    else if(arg1=="Прожектор")
            ui->openGLWidget->light_sample = 4;
    else if(arg1=="Прожектор, вкл. расчет убывания интенсивности")
            ui->openGLWidget->light_sample = 5;
    else
            ui->openGLWidget->light_sample = 6;
    ui->openGLWidget->update();
}

void MainWindow::on_comboBox_activated(int index)
{
    if(index == 0){
        ui->openGLWidget->material_sample = 1;
    }else if(index == 1){
        ui->openGLWidget->material_sample = 2;
    }else if(index == 2){
        ui->openGLWidget->material_sample = 3;
    }else{
        ui->openGLWidget->material_sample = 4;
    }
    ui->openGLWidget->update();
}

void MainWindow::on_up_light_clicked()
{
    ui->openGLWidget->lightY += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_left_light_clicked()
{
    ui->openGLWidget->lightX += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_rightZ_light_clicked()
{
    ui->openGLWidget->lightZ += 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_leftZ_light_clicked()
{
    ui->openGLWidget->lightZ -= 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_right_light_clicked()
{
    ui->openGLWidget->lightX -= 0.1;
    ui->openGLWidget->update();
}

void MainWindow::on_down_light_clicked()
{
    ui->openGLWidget->lightY -= 0.1;
    ui->openGLWidget->update();
}
