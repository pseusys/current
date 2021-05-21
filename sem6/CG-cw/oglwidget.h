#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimerEvent>
#include <QBasicTimer>

#include "lightning.h"
#include "model.h"
#include "rotas.h"

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    OGLWidget(QWidget *parent = 0);

    Rotas opera;
    Lightning zap;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    Model speaker;
    QOpenGLShaderProgram shader;
};

#endif // OGLWIDGET_H
