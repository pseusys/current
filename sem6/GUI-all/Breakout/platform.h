#ifndef PLATFORM_H
#define PLATFORM_H


#include <QImage>
#include <QRect>


class Platform {
public:
    Platform();
    ~Platform();

public:
    void resetState();
    void move();
    void setDx(int);
    QRect getRect();
    int getDX();
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    QImage& getImage();

private:
    QImage image;
    QRect rect;
    int dx;
    static const int INITIAL_X = 130;
    static const int INITIAL_Y = 360;
};

#endif // PLATFORM_H
