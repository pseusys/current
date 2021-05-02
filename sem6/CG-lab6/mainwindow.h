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
    void on_up_clicked();

    void on_left_clicked();

    void on_rightZ_clicked();

    void on_leftZ_clicked();

    void on_right_clicked();

    void on_down_clicked();

    void on_scaleXup_clicked();

    void on_scaleXdown_clicked();

    void on_scaleYup_clicked();

    void on_scaleYdown_clicked();

    void on_scaleZup_clicked();

    void on_scaleZdown_clicked();

    void on_translateLeft_clicked();

    void on_translateRight_clicked();

    void on_translateDown_clicked();

    void on_translateUp_clicked();

    void on_shape_visibility_stateChanged(int arg1);

    void on_edge_visibility_stateChanged(int arg1);

    void on_trans_visibility_stateChanged(int arg1);

    void on_coords_visibility_stateChanged(int arg1);

    void on_fineness_valueChanged(int value);

    void on_intensity_valueChanged(int value);

    void on_ortho_btn_clicked();

    void on_perspective_btn_clicked();

    void on_up_eye_clicked();

    void on_left_eye_clicked();

    void on_rightZ_eye_clicked();

    void on_leftZ_eye_clicked();

    void on_right_eye_clicked();

    void on_down_eye_clicked();

    void on_comboBoxLight_currentTextChanged(const QString &arg1);

    void on_comboBox_activated(int index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
