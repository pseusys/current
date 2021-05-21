#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

struct Material {
    QVector3D color, ambient;
    GLfloat specular, shininess, emission;
};

class Model {
public:
    Model();
    ~Model();
    void build(QString file);
    void setMaterial(int path, Material material);
    void setSquareTexture(int path, QString file);

    int start();
    void draw(int path, QOpenGLShaderProgram& program, int mode = GL_TRIANGLE_STRIP,
              const char* coordAN = "vert_coord", const char* normalAN = "vert_normal",
              const char* texturedAN = "textured", const char* texCoordAN = "vert_tex_coord", const char* textureAN = "frag_texture",
              const char* colorAN = "vert_color", const char* specularAN = "vert_specular", const char* shininessAN = "vert_shininess",
              const char* ambientAN = "vert_ambient", const char* emissionAN = "vert_emission");
    int finish();

private:
    QList<QOpenGLBuffer> VBOs;
    QHash<int, Material> materials;
    QHash<int, QOpenGLTexture*> textures;

    bool drawing = false;
    int drawn = 0;
};

#endif // MODEL_H
