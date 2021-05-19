#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "oglwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_up_eye_clicked();

    void on_left_eye_clicked();

    void on_rightZ_eye_clicked();

    void on_leftZ_eye_clicked();

    void on_right_eye_clicked();

    void on_down_eye_clicked();

    void on_up_light_clicked();

    void on_left_light_clicked();

    void on_rightZ_light_clicked();

    void on_leftZ_light_clicked();

    void on_right_light_clicked();

    void on_down_light_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
