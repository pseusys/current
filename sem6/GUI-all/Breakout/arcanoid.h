#ifndef ARCANOID_H
#define ARCANOID_H


#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include "ball.h"
#include "brick.h"
#include "platform.h"


class Arcanoid : public QWidget {
    Q_OBJECT

public:
    Arcanoid(QWidget* parent = 0);
    virtual ~Arcanoid();

    static const int WIDTH = 300;
    static const int HEIGHT = 400;

protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void drawObjects(QPainter *);
    void finishGame(QPainter *, QString);
    void moveObjects();
    void startGame();
    void pauseGame();
    void stopGame();
    void victory();
    void checkCollision();

signals:
    void finish();

private:
    int x;
    int timerId;
    static const int N_OF_BRICKS = 30;
    static const int DELAY = 10;
    Ball* ball;
    Platform* platform;
    Brick* bricks[N_OF_BRICKS];
    bool gameOver;
    bool gameWon;
    bool gameStarted;
    bool paused;
    int score = 0;
};

#endif // ARCANOID_H
