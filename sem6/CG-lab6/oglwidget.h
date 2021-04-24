#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimerEvent>
#include <QBasicTimer>
#include <cmath>

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    double angleX = 140;
    double angleY = 0;
    double angleZ = 0;
    double eyeX = 0;
    double eyeY = 0;
    double eyeZ = -4.0;
    double scaleX = 0.4;
    double scaleY = 0.4;
    double scaleZ = 0.4;
    double translateX = 0;
    double translateY = 0;
    double translateZ = 0;
    bool ortog = 0;
    bool persp = 0;
    bool shape_visible = 0;
    bool edge_visible = 0;
    bool trans_visible = 0;
    bool coords_visible = 0;
    int fineness = 4;
    int intensity = 99;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void vert();
    void plot();
    void coords();
};

#endif // OGLWIDGET_H
