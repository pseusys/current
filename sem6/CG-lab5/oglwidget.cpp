#include "oglwidget.h"

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

OGLWidget::~OGLWidget()
{

}

GLfloat vertices[] = {
    -0.8, 0,
    0, 0.8,
    0.8, 0,
    0, -0.85,

};

GLfloat colors[] = {
    0,0,0,
    0,0,0,
    0,0,0,
    1, 0.3, 0
};

const char* vertexShader = "attribute vec2 coord2d;\n"
                           "attribute vec3 color;\n"
                           "varying vec3 vColor;\n"
                           "    void main(void) {\n"

                           "    gl_Position = vec4(coord2d.x, coord2d.y, 0, 1);\n"
                           "    vColor = color;   \n"
                           "}\n";
const char* fragmentShader =
                                   "varying vec3 vColor;\n"
                                   "    void main(void) {\n"
                                   "    gl_FragColor[0] = vColor.r;\n"
                                   "    gl_FragColor[1] = vColor.g;\n"
                                   "    gl_FragColor[2] = vColor.b;\n"
                                   "}\n";

GLint attributeCoord2d;
GLint attributeColor;
GLint program;


void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderId);
    GLint compileOk = GL_FALSE;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &compileOk);
    if(compileOk == GL_FALSE){
        std::cout << "compile error\n";
    }else{
        std::cout << "compile ok\n";
    }

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &compileOk);
    if(compileOk == GL_FALSE){
        std::cout << "compile error\n";
    }else{
        std::cout << "compile ok\n";
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
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLWidget::draw(int d)
{

}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    GLuint* vboIds = new GLuint[2];
    glGenBuffers(2, vboIds);
    attributeCoord2d = glGetAttribLocation(program, "coord2d");
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeCoord2d,2,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(attributeCoord2d);

    attributeColor = glGetAttribLocation(program, "color");
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(attributeColor,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(attributeColor);

    glDrawArrays(GL_POLYGON, 0, 4);


    glDisableVertexAttribArray(attributeCoord2d);
    glDisableVertexAttribArray(attributeColor);



}



