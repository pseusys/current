#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::restart() {
    arcanoid->close();
    this->show();
}

void MainWindow::reset() {
    results->close();
    this->show();
}


void MainWindow::on_new_game_clicked() {
    arcanoid = new Arcanoid();
    arcanoid->resize(300, 400);
    arcanoid->setWindowTitle("Arcanoid");
    arcanoid->show();
    this->hide();
    connect(arcanoid, &Arcanoid::finish, this, &MainWindow::restart);
}

void MainWindow::on_results_clicked() {
    results = new Results();
    results->resize(300, 400);
    results->setWindowTitle("Results");
    results->show();
    this->hide();
    connect(results, &Results::finish, this, &MainWindow::reset);
}

void MainWindow::on_exit_clicked() {
    this->close();
}
