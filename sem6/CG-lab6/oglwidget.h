#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimerEvent>
#include <QBasicTimer>

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    GLdouble verts[16][18][3] = {};

    void vert();
    void plot();
};

#endif // OGLWIDGET_H
