#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "oglwidget.h"

OGLWidget::OGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    timer.start(250, this);
}

OGLWidget::~OGLWidget() {
    timer.stop();
}



GLfloat vertices[] = {
    -0.8, 0,
    0, 0.8,
    0.8, 0,
    0.2, -0.7,
    0, -0.8,
    -0.2, -0.7
};

GLfloat colors[] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    1, 0.3, 0,
    1, 0.3, 0,
    1, 0.3, 0
};

GLint program;

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    initializeOpenGLFunctions();

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    try {
        vShaderFile.open("./fire.vert");
        fShaderFile.open("./fire.frag");
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch(std::ifstream::failure& e) {
        std::cout << "read not ok" << std::endl;
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vShaderCode, NULL);
    glCompileShader(vertexShaderId);
    GLint compileOk = GL_FALSE;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &compileOk);
    if(compileOk == GL_FALSE){
        std::cout << "vertex compile error\n";
    }else{
        std::cout << "vertex compile ok\n";
    }

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &compileOk);
    if(compileOk == GL_FALSE){
        std::cout << "fragment compile error\n";
    }else{
        std::cout << "fragment compile ok\n";
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShaderId);
    glAttachShader(program, fragmentShaderId);
    glLinkProgram(program);
    GLint linkOk = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkOk);
    if(linkOk == GL_FALSE){
        std::cout << "link error\n";
    }else{
        std::cout << "link ok\n";
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    glClearColor(0,0,0,1);
    initUniforms();
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLWidget::initUniforms() {
    for (int i = 0; i < 10; i += 2) {
        this->milestones[i] = this->width() / 2;
        this->milestones[i + 1] = this->height() / 10;
    }
    this->radius = 75;
    this->intens = 1;
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    GLuint* vboIds = new GLuint[2];
    glGenBuffers(2, vboIds);

    GLint attributeCoord2d = glGetAttribLocation(program, "coord2d");
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeCoord2d,2,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(attributeCoord2d);

    GLint attributeColor = glGetAttribLocation(program, "color");
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeColor,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(attributeColor);

    GLint uniformMilestones = glGetUniformLocation(program, "milestones");
    glUniform2fv(uniformMilestones, 5, (const GLfloat*) milestones);

    GLint uniformRadius = glGetUniformLocation(program, "radius");
    glUniform1i(uniformRadius, radius);

    GLint intensRadius = glGetUniformLocation(program, "intens");
    glUniform1f(intensRadius, intens);

    glDrawArrays(GL_POLYGON, 0, sizeof(vertices) / sizeof(GLfloat));

    glDisableVertexAttribArray(attributeCoord2d);
    glDisableVertexAttribArray(attributeColor);
}

void OGLWidget::timerEvent(QTimerEvent* ev)
{
    if (ev->timerId() != timer.timerId()) {
        QWidget::timerEvent(ev);
        return;
    }
    callBack();
    update();
}
