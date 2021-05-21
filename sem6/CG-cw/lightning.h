#ifndef LIGHTNING_H
#define LIGHTNING_H


#include <QOpenGLShaderProgram>

class Lightning {
public:
    Lightning();
    void light(QOpenGLShaderProgram& program, const char* coordAN = "light_coord", const char* colorAN = "light_color", const char* strengthAN = "light_strength");
    void translateBy(float x, float y, float z);

    QVector3D lightPos;

private:
    QVector3D lightColor;
    GLfloat lightStrength;
};

#endif // LIGHTNING_H
