#ifndef RESULTS_H
#define RESULTS_H

#include <QWidget>
#include <QPushButton>

class Results : public QWidget
{
    Q_OBJECT

public:
    explicit Results(QWidget *parent = nullptr);

signals:
    void finish();

};

#endif // RESULTS_H
