#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "arcanoid.h"
#include "results.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

private slots:
    void restart();
    void reset();
    void on_new_game_clicked();
    void on_results_clicked();
    void on_exit_clicked();

private:
    Ui::MainWindow* ui;
    Arcanoid* arcanoid;
    Results* results;
};

#endif // MAINWINDOW_H
