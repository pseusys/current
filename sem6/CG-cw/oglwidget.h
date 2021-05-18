#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimerEvent>
#include <QBasicTimer>

#include "model.h"
#include "rotas.h"

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();

    GLfloat milestones[10], intens;
    GLint radius;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void timerEvent(QTimerEvent* ev) override;

private:
    Model speaker;
    Rotas opera;
    QBasicTimer timer;

    void initUniforms();
    void callBack();
};

#endif // OGLWIDGET_H
