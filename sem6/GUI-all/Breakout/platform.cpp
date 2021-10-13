#include "platform.h"
#include <QDebug>


Platform::Platform() {
    dx = 0;
    image.load(":/platform.png");

    rect = image.rect();
    resetState();
}


Platform::~Platform() {
    qDebug() << ("Paddle deleted");
}


void Platform::setDx(int x) {
    dx = x;
}


int Platform::getDX() {
    return dx;
}


void Platform::move() {
    int x = rect.x() + dx;
    int y = rect.top();

    rect.moveTo(x, y);
}


int Platform::getX() {
    return rect.x();
}


int Platform::getY() {
    return rect.y();
}


int Platform::getWidth() {
    return rect.width();
}


int Platform::getHeight() {
    return rect.height();
}


void Platform::resetState() {
    rect.moveTo(INITIAL_X, INITIAL_Y);
}


QRect Platform::getRect() {
    return rect;
}


QImage& Platform::getImage() {
    return image;
}
