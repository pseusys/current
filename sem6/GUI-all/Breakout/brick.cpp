#include "brick.h"
#include <QDebug>


Brick::Brick(int x, int y) {
    lives = QRandomGenerator::global()->bounded(1, 4);
    setImage(lives);
    destroyed = false;
    rect = image.rect();
    rect.translate(x, y);
}


Brick::~Brick() {
    qDebug() << ("Brick deleted");
}


QRect Brick::getRect() {
    return rect;
}


void Brick::setRect(QRect rect) {
    this->rect = rect;
}


QImage& Brick::getImage() {
    return image;
}


bool Brick::isDestroyed() {
    return destroyed;
}


void Brick::setImage(int lives) {
    QString imageName = ":/brickie.png";
    imageName.insert(9, QChar(lives + '0'));
    image.load(imageName);
}


void Brick::setDestroyed(bool destroyed) {
    if (destroyed) {
        lives--;
        if (lives > 0) {
            setImage(lives);
        }
        else {
            this->destroyed = true;
        }
    } else {
        this->destroyed = false;
    }
}
