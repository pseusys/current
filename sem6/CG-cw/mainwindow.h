#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "oglwidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_y_plus_eye_clicked();
    void on_y_minus_eye_clicked();

    void on_x_plus_eye_clicked();
    void on_x_minus_eye_clicked();

    void on_z_plus_eye_clicked();
    void on_z_minus_eye_clicked();

    void on_rot_right_eye_clicked();
    void on_rot_left_eye_clicked();

    void on_y_plus_light_clicked();
    void on_y_minus_light_clicked();

    void on_x_plus_light_clicked();
    void on_x_minus_light_clicked();

    void on_z_plus_light_clicked();
    void on_z_minus_light_clicked();

private:
    void updateCamera();
    void updateLight();
    QString qFuzzy(float num);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
