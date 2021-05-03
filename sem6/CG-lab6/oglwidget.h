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
    double angleX = 135;
    double angleY = 0;
    double angleZ = 0;
    double eyeX = 0;
    double eyeY = 0;
    double eyeZ = -4.0;
    double scaleX = 1;
    double scaleY = 1;
    double scaleZ = 1;
    double translateX = 0;
    double translateY = 0;
    double translateZ = 0;
    bool ortog = 0;
    bool shape_visible = 1;
    bool edge_visible = 1;
    bool trans_visible = 0;
    bool coords_visible = 0;
    int fineness = 4;
    int intensity = 99;
    int light_sample = 4;
    int material_sample = 1;
    float lightX = 1.0;
    float lightY = 1.0;
    float lightZ = 1.0;


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
