#ifndef LIGHTNING_H
#define LIGHTNING_H


#include <QOpenGLShaderProgram>

class Lightning {
public:
    Lightning();
    void light(QOpenGLShaderProgram* program, const char* coordAN = "light_coord", const char* colorAN = "light_color", const char* strengthAN = "light_strength");
    void color(QOpenGLShaderProgram* program, const char* colorAN = "vert_color", const char* specularAN = "vert_specular", const char* shininessAN = "vert_shininess");
    void translateBy(float x, float y, float z);

    QVector3D lightPos;

private:
    QVector3D lightColor, materialColor;
    GLfloat lightStrength, materialShininess, materialSpecular;
};

#endif // LIGHTNING_H
