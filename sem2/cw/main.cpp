#include <QApplication>

#include "window/window.h"
#include "image/image.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));

    ImageViewer imageViewer;
    imageViewer.show();

    imageViewer.loadFile("/home/alex/CLionProjects/course_work/MARBLES.bmp");

    return QApplication::exec();
}