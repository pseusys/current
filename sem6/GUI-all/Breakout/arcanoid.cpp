#include <QPainter>
#include <QDateTime>
#include <QApplication>
#include <iostream>
#include <fstream>
#include "arcanoid.h"


Arcanoid::Arcanoid(QWidget* parent) : QWidget(parent) {
    x = 0;
    gameOver = false;
    gameWon = false;
    paused = false;
    gameStarted = false;
    ball = new Ball();
    platform = new Platform();

    int k = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            bricks[k] = new Brick(j * 40 + 30, i * 10 + 50);
            k++;
        }
    }
}


Arcanoid::~Arcanoid() {
    delete ball;
    delete platform;

    for (int i = 0; i < N_OF_BRICKS; i++) {
        delete bricks[i];
    }
}


void Arcanoid::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);

    QPainter painter(this);
    if (gameOver) {
        finishGame(&painter, "Game lost, score: " + QString::number(score));
    } else if (gameWon) {
        finishGame(&painter, "Victory");
    } else if (paused) {
        finishGame(&painter, "Paused");
    } else {
        drawObjects(&painter);
    }
}


void Arcanoid::drawObjects(QPainter* painter) {
    QFont font("Courier", 13, QFont::DemiBold);
    QFontMetrics fm(font);

    QString message = "Score: " + QString::number(score);
    int textWidth = fm.boundingRect(message).width();

    painter->setFont(font);
    painter->drawText(0.95 * width() - textWidth, 0.99 * height() - fm.height(), textWidth + 1, fm.height(), 0, message);
    painter->drawImage(ball->getRect(), ball->getImage());
    painter->drawImage(platform->getRect(), platform->getImage());

    for (int i = 0; i < N_OF_BRICKS; i++) {
        if (!bricks[i]->isDestroyed()) {
            painter->drawImage(bricks[i]->getRect(), bricks[i]->getImage());
        }
    }
}


void Arcanoid::timerEvent(QTimerEvent* e) {
    Q_UNUSED(e);

    moveObjects();
    checkCollision();
    repaint();
}


void Arcanoid::startGame() {
    if (!gameStarted) {
        ball->resetState();
        platform->resetState();

        for (int i = 0; i < N_OF_BRICKS; i++) {
            bricks[i]->setDestroyed(false);
        }

        gameOver = false;
        gameWon = false;
        gameStarted = true;
        timerId = startTimer(DELAY);
    }
}


void Arcanoid::pauseGame() {
    if (paused) {
        timerId = startTimer(DELAY);
        paused = false;
    } else {
        paused = true;
        killTimer(timerId);
        repaint();
    }
}


void Arcanoid::stopGame() {
    killTimer(timerId);
    gameOver = true;
    gameStarted = false;
}


void Arcanoid::victory() {
    killTimer(timerId);
    gameWon = true;
    gameStarted = false;
}

void commitResult(int score) {
    QDateTime current = QDateTime::currentDateTime();

    try {
        std::ofstream fileStream;
        fileStream.open("./results.400s", std::ios_base::app);
        fileStream << current.toString("dd.MM.yyyy-hh:mm:ss").toStdString() << " " << QString::number(score).toStdString() << " ";
        fileStream.close();
    } catch(std::ifstream::failure& e) {
        std::cout << "write not ok" << std::endl;
    }
}


void Arcanoid::finishGame(QPainter* painter, QString message) {
    QFont font("Courier", 15, QFont::DemiBold);
    QFontMetrics fm(font);

    int textWidth = fm.boundingRect(message).width();

    painter->setFont(font);
    int h = height();
    int w = width();

    painter->translate(QPoint(w / 2, h / 2));
    painter->drawText(-textWidth / 2, 0, message);

    commitResult(score);

    if (!paused) {
        QTimer::singleShot(2000, this, SIGNAL(finish()));
    }
}


void Arcanoid::moveObjects() {
    ball->autoMove();

    if ((platform->getX() < 0 && platform->getDX() < 0) ||
            (platform->getX() + platform->getWidth() > this->size().width() && platform->getDX() > 0)) {
        platform->setDx(0);
    }
    platform->move();
}


void Arcanoid::keyReleaseEvent(QKeyEvent* e) {
    int dx = 0;
    switch (e->key()) {
    case Qt::Key_Left:
        platform->setDx(dx);
        break;
    case Qt::Key_Right:
        platform->setDx(dx);
        break;
    }
}


void Arcanoid::keyPressEvent(QKeyEvent* e) {
    int dx = 0;
    switch (e->key()) {
    case Qt::Key_Left:
        dx = -1;
        platform->setDx(dx);
        break;
    case Qt::Key_Right:
        dx = 1;
        platform->setDx(dx);
        break;
    case Qt::Key_P:
        pauseGame();
        break;
    case Qt::Key_Space:
        startGame();
        break;
    case Qt::Key_Escape:
        emit finish();
        break;
    default:
        QWidget::keyPressEvent(e);
    }
}


void Arcanoid::checkCollision() {
    if (ball->getRect().bottom() > HEIGHT) {
        stopGame();
    }

    for (int i = 0, j = 0; i < N_OF_BRICKS; i++) {
        if (bricks[i]->isDestroyed()) {
            j++;
        }
        if (j == N_OF_BRICKS) {
            victory();
        }
    }

    if ((ball->getRect()).intersects(platform->getRect())) {
        int paddleLPos = platform->getRect().left();
        int ballLPos = ball->getRect().left();

        int first = paddleLPos + 8;
        int second = paddleLPos + 16;
        int third = paddleLPos + 24;
        int fourth = paddleLPos + 32;

        if (ballLPos < first) {
            ball->setXDir(-1);
            ball->setYDir(-1);
        }
        if (ballLPos >= first && ballLPos < second) {
            ball->setXDir(-1);
            ball->setYDir(-1 * ball->getYDir());
        }
        if (ballLPos >= second && ballLPos < third) {
            ball->setXDir(0);
            ball->setYDir(-1);
        }
        if (ballLPos >= third && ballLPos < fourth) {
            ball->setXDir(1);
            ball->setYDir(-1 * ball->getYDir());
        }
        if (ballLPos > fourth) {
            ball->setXDir(1);
            ball->setYDir(-1);
        }
    }

    for (int i = 0; i < N_OF_BRICKS; i++) {
        if ((ball->getRect()).intersects(bricks[i]->getRect())) {
            int ballLeft = ball->getRect().left();
            int ballHeight = ball->getRect().height();
            int ballWidth = ball->getRect().width();
            int ballTop = ball->getRect().top();

            QPoint pointRight(ballLeft + ballWidth + 1, ballTop);
            QPoint pointLeft(ballLeft - 1, ballTop);
            QPoint pointTop(ballLeft, ballTop - 1);
            QPoint pointBottom(ballLeft, ballTop + ballHeight + 1);

            if (!bricks[i]->isDestroyed()) {
                if(bricks[i]->getRect().contains(pointRight)) {
                    ball->setXDir(-1);
                } else if(bricks[i]->getRect().contains(pointLeft)) {
                    ball->setXDir(1);
                }

                if(bricks[i]->getRect().contains(pointTop)) {
                    ball->setYDir(1);
                } else if(bricks[i]->getRect().contains(pointBottom)) {
                    ball->setYDir(-1);
                }

                bricks[i]->setDestroyed(true);
                score++;
            }
        }
    }
}
