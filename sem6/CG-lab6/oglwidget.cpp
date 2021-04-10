#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "oglwidget.h"

OGLWidget::OGLWidget(QWidget *parent): QOpenGLWidget(parent) {}
OGLWidget::~OGLWidget() {}



GLint program;

void OGLWidget::initializeGL() {
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
    std::cout << ((compileOk == GL_FALSE) ? "vertex compile error" : "vertex compile ok") << std::endl;

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &compileOk);
    std::cout << ((compileOk == GL_FALSE) ? "fragment compile error" : "fragment compile ok") << std::endl;

    program = glCreateProgram();
    glAttachShader(program, vertexShaderId);
    glAttachShader(program, fragmentShaderId);
    glLinkProgram(program);
    GLint linkOk = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkOk);
    std::cout << ((linkOk == GL_FALSE) ? "link error" : "link ok") << std::endl;

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    glClearColor(0,0,0,1);
    vert();
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*glUseProgram(program);
    GLuint* vboId = new GLuint();
    glGenBuffers(1, vboId);

    GLint attributeCoord3d = glGetAttribLocation(program, "coord3d");
    glBindBuffer(GL_ARRAY_BUFFER, *vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeCoord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attributeCoord3d);*/

    glTranslated(0.0, -0.25, 0.0);
    glScaled(0.5, 0.5, 0.5);
    glRotated(45.0, 1.0, 0.0, 0.0);
    plot();

    //glDisableVertexAttribArray(attributeCoord3d);
    //delete vboId;
}
