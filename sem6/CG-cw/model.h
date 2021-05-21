#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>


class Model {
public:
    Model();
    ~Model();
    void build(QString file, QString general, QString textured);
    void setSquareTexture(int path, QString file);
    QOpenGLShaderProgram* getShader(int path);

    int start();
    void draw(int num, QOpenGLShaderProgram* program, int mode = GL_TRIANGLE_STRIP,
              const char* coordAN = "vert_coord", const char* normalAN = "vert_normal",
              const char* texCoordAN = "vert_tex_coord", const char* textureAN = "frag_texture");
    int finish();

private:
    QList<QOpenGLBuffer> VBOs;
    QHash<int, QOpenGLTexture*> textures;
    QOpenGLBuffer simpleTextureCoords;
    QOpenGLShaderProgram generalS, texturedS;

    bool drawing = false;
    int drawn = 0;
};

#endif // MODEL_H
