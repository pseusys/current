#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>

class OGLWidget : public QOpenGLWidget
{
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();

    double xA = -0.8;
    double yA = 0;
    double xB = 0.8;
    double yB = 0.07;
    int n = 2;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void draw(std::vector< std::vector<double> > array);
    std::vector< std::vector<double> > fract(std::vector< std::vector<double> > array);
    std::vector< std::vector<double> > traf(double xA, double yA, double xB, double yB);

};

#endif // OGLWIDGET_H
