#ifndef BRICK_H
#define BRICK_H


#include <QImage>
#include <QRect>
#include <QRandomGenerator>


class Brick {
public:
    Brick(int, int);
    ~Brick();

public:
    bool isDestroyed();
    void setDestroyed(bool);
    QRect getRect();
    void setRect(QRect);
    QImage& getImage();
    void setImage(int lives);

private:
    QImage image;
    QRect rect;
    bool destroyed;
    int lives;
};

#endif // BRICK_H
