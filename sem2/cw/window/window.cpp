#include "window.h"

int ImageViewer::isSupported = 1;

ImageViewer::ImageViewer() : imageLabel(new QLabel), scrollArea(new QScrollArea) {
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    createActions();

    resize(800, 600);

    Image::root = this;
}


bool ImageViewer::loadFile(const QString &fileName) {
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));

    scrollArea->setVisible(true);
    updateActions();

    imageLabel->adjustSize();

    setWindowFilePath(fileName);

    Image::loadImage(fileName.toLatin1().data());

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::open() {
    QFileDialog dialog(this, tr("Open File"));
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = QImageReader::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
            mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/bmp");

    isSupported = 1;

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first()));
}

void ImageViewer::saveAs() {
    bool ok;
    QString name = QInputDialog::getText(this, Tools::tr("Name"), Tools::tr("New image name:"), QLineEdit::Normal, 0, &ok);
    name += ".bmp";
    Image::flushImage(Image::bitMap, name.toStdString().c_str());
}

void ImageViewer::reload() {
    this->loadFile(Image::name);
}

void ImageViewer::invertColors() {
    Tools::toolID = 0;
    char n = INVERTTOOL;
    Tools::multiTool(&n, this);
}

void ImageViewer::toBnW() {
    Tools::toolID = 0;
    char n = BNWTOOL;
    Tools::multiTool(&n, this);
}

void ImageViewer::resizeImage() {
    Tools::toolID = 0;
    char n = RESIZETOOL;
    Tools::multiTool(&n, this);
}

void ImageViewer::drawLine() {
    Tools::toolID = 0;
    char n = LINETOOL;
    Tools::multiTool(&n, this);
}

void ImageViewer::about() {
    QMessageBox::about(this, tr("About App"),
                       tr("<p>This program may be used to draw lines, apply BnW filter, invert colors and resize image."
                          "Do not forget to  select two points for every tool after selecting it.</p>"
                          "<p>Made by Sergeev Alexander, 8381.</p>"));
}

void ImageViewer::aboutImage() {
    QMessageBox::about(this, tr("About Image"),
                       tr("<p><b>File Header:</b><p/><p>  Markers: %1%2</p><p>  File size: %3</p><p>  PA position: %4</p><p>  Reserved: %5 %6</p><p>"
                          "<p><b>Info Header:</b></p><p>  Height: %7</p><p>  Width: %8</p><p>  Colors: %9</p><p>  Compression: %10</p><p>  PA size: %11</p><p>  IH size: %12</p><p>  Planes: %13</p><p>  BitsPerPixel: %14</p><p>  hPPM: %15</p><p>  vPPM: %16</p><p>  Important colors: %17</p>"
                          "<p><b>Dump IH:</b> %18</p>"
                          "<p>Gap 1: %19</p>"
                          "<p><b>Color Table:</b> %20</p>"
                          "<p>Gap 2: %21</p>"
                          "<p><b>Pixel Array:</b> %22</p>"
                          "<p>Gap 3: %23</p>")
                               .arg(Image::bitMap->fileHeader.marker1)
                               .arg(Image::bitMap->fileHeader.marker2)
                               .arg(Image::bitMap->fileHeader.fileSize)
                               .arg(Image::bitMap->fileHeader.pixelArrayPosition)
                               .arg(Image::bitMap->fileHeader.reserved1)
                               .arg(Image::bitMap->fileHeader.reserved2)
                               .arg(Image::bitMap->infoHeader.height)
                               .arg(Image::bitMap->infoHeader.width)
                               .arg(Image::bitMap->infoHeader.colorsNumber)
                               .arg(Image::bitMap->infoHeader.compression)
                               .arg(Image::bitMap->infoHeader.pixelArraySize)
                               .arg(Image::bitMap->infoHeader.infoHeaderSize)
                               .arg(Image::bitMap->infoHeader.planes)
                               .arg(Image::bitMap->infoHeader.bitsPerPixel)
                               .arg(Image::bitMap->infoHeader.horizontalPixelPerMeter)
                               .arg(Image::bitMap->infoHeader.verticalPixelPerMeter)
                               .arg(Image::bitMap->infoHeader.importantColorsNumber)
                               .arg(Image::bitMap->contains.dump_ih)
                               .arg(Image::bitMap->contains.gap0)
                               .arg(Image::bitMap->contains.ct)
                               .arg(Image::bitMap->contains.gap1)
                               .arg(Image::bitMap->infoHeader.pixelArraySize)
                               .arg(Image::bitMap->contains.gap2));
}

void ImageViewer::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);

    reloadAct = fileMenu->addAction(tr("Reload &Again"), this, &ImageViewer::reload);
    reloadAct->setShortcut(tr("Ctrl+A"));
    reloadAct->setEnabled(false);

    saveAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAct->setShortcut(QKeySequence::SaveAs);
    saveAct->setEnabled(false);

    fileMenu->addSeparator();

    exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));

    invertAct = toolsMenu->addAction(tr("&Invert colors"), this, &ImageViewer::invertColors);
    invertAct->setEnabled(false);
    invertAct->setShortcut(tr("Ctrl+I"));

    toBnWAct = toolsMenu->addAction(tr("To &Black and White"), this, &ImageViewer::toBnW);
    toBnWAct->setEnabled(false);
    toBnWAct->setShortcut(tr("Ctrl+B"));

    resizeAct = toolsMenu->addAction(tr("&Resize"), this, &ImageViewer::resizeImage);
    resizeAct->setEnabled(false);
    resizeAct->setShortcut(tr("Ctrl+R"));

    drawLineAct = toolsMenu->addAction(tr("Draw &Line"), this, &ImageViewer::drawLine);
    drawLineAct->setEnabled(false);
    drawLineAct->setShortcut(tr("Ctrl+L"));

    toolsMenu->addAction(tr("&Rotate"), this, &Image::rotate);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    aboutImageAct = helpMenu->addAction(tr("Abo&ut Image"), this, &ImageViewer::aboutImage);
    aboutImageAct->setEnabled(false);
    helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
    helpMenu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}

void ImageViewer::updateActions() {
    saveAct->setEnabled(!image.isNull());
    reloadAct->setEnabled(!image.isNull());

    bool proofer = (!image.isNull()) && isSupported;
    invertAct->setEnabled(proofer);
    toBnWAct->setEnabled(proofer);
    resizeAct->setEnabled(proofer);
    drawLineAct->setEnabled(proofer);
    aboutImageAct->setEnabled(proofer);
}

void ImageViewer::mousePressEvent(QMouseEvent* ev) {
    const QPoint p = ev->pos();
    int xAbsolute = scrollArea->horizontalScrollBar()->value() + p.x();
    int yAbsolute = scrollArea->verticalScrollBar()->value() + p.y();
    printf("Coordinates got: %i, %i\n", xAbsolute, yAbsolute);
    int* value = (int*) malloc(2 * sizeof(int));
    value[0] = xAbsolute;
    value[1] = yAbsolute;

    if ((xAbsolute < Image::bitMap->infoHeader.width) && (yAbsolute < Image::bitMap->infoHeader.height)) {
        Tools::multiTool(value, this);
    } else {
        Tools::toolID = NOTOOL;
        Tools::step = 0;
        QString message;
        if (xAbsolute > Image::bitMap->infoHeader.width) {
            message = tr("Out of image: x%1 < x%2 (width)").arg(xAbsolute).arg(Image::bitMap->infoHeader.width);
        } else {
            message = tr("Out of image: y%1 < y%2 (height)").arg(yAbsolute).arg(Image::bitMap->infoHeader.height);
        }
        statusBar()->showMessage(message);
    }
}

Pixel* ImageViewer::setColor(QString dialogName) {
    const QColorDialog::ColorDialogOptions options = QFlag(0);
    const QColor color = QColorDialog::getColor(Qt::white, this, dialogName, options);

    if (color.isValid()) {
        //colorLabel->setText(color.name());
        //colorLabel->setPalette(QPalette(color));
        //colorLabel->setAutoFillBackground(true);
    }

    auto pixel = (Pixel*) malloc(sizeof(Pixel));
    pixel->r = (unsigned char) color.red();
    pixel->g = (unsigned char) color.green();
    pixel->b = (unsigned char) color.blue();
    printf("Got color: %i %i %i\n", pixel->r, pixel->g, pixel->b);
    return pixel;
}
