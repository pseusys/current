#include "oglwidget.h"
#include <cmath>
#include <vector>

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

OGLWidget::~OGLWidget()
{

}

void OGLWidget::initializeGL()
{
    glClearColor(1,1,1,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



std::vector<double> vectorize(std::vector<double> base, std::vector<double> hComp, std::vector<double> vComp, int hMod, int vMod) {
    return {base[0] + hMod*hComp[0] + vMod*vComp[0], base[1] + hMod*hComp[1] + vMod*vComp[1]};
}

std::vector<std::vector<double>> OGLWidget::traf(double xA, double yA, double xB, double yB)
{
    double delX = xB - xA;
    double delY = yB - yA;
    std::vector<double> hUnit = {delX / 9, delY / 9};
    std::vector<double> vUnit = {-delY / 9, delX / 9};
    std::vector<double> base = {xA, yA};
    std::vector<std::vector<double>> res = {
        base,
        vectorize(base, hUnit, vUnit, 2, 1),
        vectorize(base, hUnit, vUnit, 3, 2),
        vectorize(base, hUnit, vUnit, 3, 1),
        vectorize(base, hUnit, vUnit, 5, 1),
        vectorize(base, hUnit, vUnit, 6, 3),
        vectorize(base, hUnit, vUnit, 7, 1),
        {xB, yB}
    };
    return res;
}



void OGLWidget::fract(int factor, double xA, double yA, double xB, double yB){
    std::vector<std::vector<double>> fractal = traf(xA, yA, xB, yB);
    if (factor == 0) draw(fractal);
    else for (unsigned int i = 0; i < fractal.size() - 1; i++) fract(factor - 1, fractal[i][0], fractal[i][1], fractal[i + 1][0], fractal[i + 1][1]);

}

void OGLWidget::draw( std::vector<std::vector<double>> array)
{
        glColor3f(0, 0, 1);
        glLineWidth(4);
        glBegin(GL_LINE_STRIP);
            for (unsigned int i = 0; i < array.size(); i++) glVertex2f(array[i][0], array[i][1]);
        glEnd();
}



void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fract(n, xA, yA, xB, yB);
}
