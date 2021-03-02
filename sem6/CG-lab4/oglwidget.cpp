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
std::vector< std::vector<double> > OGLWidget::traf(double xA, double yA, double xB, double yB)
{
    double delX=xB-xA;
    double delY=yB-yA;
    std::vector< std::vector<double> > res = {{xA,xA+delX/5,xA+delX/3,xA+delX/3,xA+delX/1.875,xA+delX/1.5,xA+delX/1.25,xB},
    {yA,yA+delY*2,yA+delY*5,yA+delY*2,yA+delY*2,yA+delY*7,yA+delY*2,yB}};
    return res;
}

std::vector< std::vector<double> > OGLWidget::fract( std::vector< std::vector<double> > array){
    for(int i=1; i<n;i++){


    }
}

void OGLWidget::draw( std::vector< std::vector<double> > array)
{
        glColor3f(0, 0, 1);
        glLineWidth(4);
        glBegin(GL_LINES);
            glVertex2f(array[0][0],array[1][0]);
            glVertex2f(array[0][1],array[1][1]);
            glVertex2f(array[0][1],array[1][1]);
            glVertex2f(array[0][2],array[1][2]);
            glVertex2f(array[0][2],array[1][2]);
            glVertex2f(array[0][3],array[1][3]);
            glVertex2f(array[0][3],array[1][3]);
            glVertex2f(array[0][4],array[1][4]);
            glVertex2f(array[0][4],array[1][4]);
            glVertex2f(array[0][5],array[1][5]);
            glVertex2f(array[0][5],array[1][5]);
            glVertex2f(array[0][6],array[1][6]);
            glVertex2f(array[0][6],array[1][6]);
            glVertex2f(array[0][7],array[1][0]);
        glEnd();
}


void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::vector< std::vector<double> > array = traf(xA,yA,xB,yB);

    draw(traf(xA,yA,xB,yB));
}



