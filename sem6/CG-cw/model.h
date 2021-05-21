#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>


class Model {
public:
    Model();
    ~Model();
    void build(QString file);
    void setSquareTexture(int path, QString file);

    int start();
    void draw(int path, QOpenGLShaderProgram& program, int mode = GL_TRIANGLE_STRIP,
              const char* coordAN = "vert_coord", const char* normalAN = "vert_normal",
              const char* texturedAN = "textured", const char* texCoordAN = "vert_tex_coord", const char* textureAN = "frag_texture");
    void color(int path, QOpenGLShaderProgram& program,
               const char* colorAN = "vert_color", const char* specularAN = "vert_specular", const char* shininessAN = "vert_shininess");
    int finish();

private:
    QList<QOpenGLBuffer> VBOs;
    QHash<int, QOpenGLTexture*> textures;
    QOpenGLBuffer simpleTextureCoords;

    bool drawing = false;
    int drawn = 0;
};

#endif // MODEL_H
