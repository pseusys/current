#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    int d = 1;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void draw(int d);
};

#endif // OGLWIDGET_H
