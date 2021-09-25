#ifndef COURSE_WORK_WINDOW_H
#define COURSE_WORK_WINDOW_H

class Tools;

#include <QtWidgets>
#include "../tools/tools.h"
#include "../image/image.h"

#include <QMainWindow>
#include <QImage>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;

class ImageViewer : public QMainWindow {
Q_OBJECT

public:
    ImageViewer();
    bool loadFile(const QString &);
    Pixel* setColor(QString dialogName);
    static int isSupported;
    void updateActions();

private slots:
    void open();
    void reload();
    void saveAs();

    void invertColors();
    void toBnW();
    void resizeImage();
    void drawLine();

    void about();
    void aboutImage();

private:
    void createActions();

    void mousePressEvent( QMouseEvent* ev ) override;

    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;

    QAction *openAct;
    QAction *reloadAct;
    QAction *saveAct;
    QAction *exitAct;

    QAction *invertAct;
    QAction *toBnWAct;
    QAction *resizeAct;
    QAction *drawLineAct;

    QAction *aboutImageAct;
};

#endif //COURSE_WORK_WINDOW_H
