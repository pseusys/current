#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_up_eye_clicked()
{

}

void MainWindow::on_left_eye_clicked()
{

}

void MainWindow::on_rightZ_eye_clicked()
{

}

void MainWindow::on_leftZ_eye_clicked()
{

}

void MainWindow::on_right_eye_clicked()
{

}

void MainWindow::on_down_eye_clicked()
{

}

void MainWindow::on_up_light_clicked()
{

}

void MainWindow::on_left_light_clicked()
{

}

void MainWindow::on_rightZ_light_clicked()
{

}

void MainWindow::on_leftZ_light_clicked()
{

}

void MainWindow::on_right_light_clicked()
{

}

void MainWindow::on_down_light_clicked()
{

}
