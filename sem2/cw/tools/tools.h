#ifndef COURSE_WORK_TOOLS_H
#define COURSE_WORK_TOOLS_H

class ImageViewer;

#include "../window/window.h"
#include "../image/image.h"

#include <QtWidgets/QInputDialog>
#include <QtCore/QDir>
#include <QtCore/QStringList>

#define INVERTTOOL 1
#define BNWTOOL 2
#define RESIZETOOL 3
#define LINETOOL 4
#define NOTOOL 42

class Tools : public QObject {
    Q_OBJECT

public:
    static char toolID;
    static int step;

    static int upperX, upperY, lowerX, lowerY;

    static int BnWAlgorythm;

    static int scalePercent;
    static Pixel *color;
    static int centerPosition;

    static int thickness;

    static void multiTool(void *data, ImageViewer *iv);

private:
    static void invertTool(void* data);
    static void BnWTool(void* data, ImageViewer *iv);
    static void resizeTool(ImageViewer *iv);
    static void lineTool(void* data, ImageViewer *iv);
};

#endif //COURSE_WORK_TOOLS_H
