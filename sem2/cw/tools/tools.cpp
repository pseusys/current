#include "tools.h"

char Tools::toolID = NOTOOL;
int Tools::step = 0;

int Tools::upperX, Tools::upperY, Tools::lowerX, Tools::lowerY;

int Tools::BnWAlgorythm;

int Tools::scalePercent;
Pixel* Tools::color;
int Tools::centerPosition;

int Tools::thickness;

void abort(int err) {
    if (err) printf("Operation aborted!\n");
    Tools::step = 0;
    Tools::toolID = NOTOOL;
}

void Tools::invertTool(void* data) {
    switch (step) {
        case 1:
            upperX = ((int*) data)[0];
            upperY = ((int*) data)[1];
            free(data);
            break;
        case 2:
            lowerX = ((int*) data)[0];
            lowerY = ((int*) data)[1];
            free(data);

            Image::invertColors();

            abort(0);
            break;
        default:
            printf("too many steps\n");
            abort(1);
            break;
    }
}

void Tools::BnWTool(void* data, ImageViewer* iv) {
    switch (step) {
        case 1:
            upperX = ((int*) data)[0];
            upperY = ((int*) data)[1];
            free(data);
            break;
        case 2: {
            lowerX = ((int*) data)[0];
            lowerY = ((int*) data)[1];
            free(data);

            QStringList items;
            items << Tools::tr("Normal") << Tools::tr("Fast");
            bool ok;
            QString item = QInputDialog::getItem(iv, Tools::tr("Mode"),
                    Tools::tr("Black'n'White mode:"), items, 0, false, &ok);

            if (ok) {
                BnWAlgorythm = items.indexOf(item);
                Image::bnw();
                abort(0);
            } else {
                abort(1);
            }

            break;
        }
        default:
            printf("too many steps\n");
            abort(1);
            break;
    }
}

void Tools::resizeTool(ImageViewer* iv) {
    bool ok1;
    scalePercent = QInputDialog::getInt(iv, Tools::tr("Select percent:"), Tools::tr("Resize in percent:"), 0, -100, 100, 1, &ok1);

    if (!ok1) {
        abort(1);
        return;
    }

    if (scalePercent > 0)
        color = iv->setColor("Select color to fill:");

    QStringList items;
    items << Tools::tr("Upper Right") << Tools::tr("Upper Left") << Tools::tr("Center") << Tools::tr("Lower Right") << Tools::tr("Lower Left");
    bool ok2;
    QString item = QInputDialog::getItem(iv, Tools::tr("Position of resizing"), Tools::tr("Position:"), items, 2, false, &ok2);

    if (!ok2) {
        abort(1);
        return;
    }

    if (item == "Upper Right") {
        centerPosition = -2;
    } else if (item == "Upper Left") {
        centerPosition = -1;
    } else if (item == "Center") {
        centerPosition = 0;
    } else if (item == "Lower Left") {
        centerPosition = 1;
    } else if (item == "Lower Right") {
        centerPosition = 2;
    }

    Image::resize();

    abort(0);
}

void Tools::lineTool(void* data, ImageViewer* iv) {
    switch (step) {
        case 1:
            upperX = ((int*) data)[0];
            upperY = ((int*) data)[1];
            free(data);
            break;
        case 2: {
            lowerX = ((int *) data)[0];
            lowerY = ((int *) data)[1];
            free(data);

            color = iv->setColor("Line Color");

            bool ok;
            thickness = QInputDialog::getInt(iv, Tools::tr("Select thickness:"), Tools::tr("Thickness of line:"), 0, 1, 1000, 1, &ok);

            if (ok) {
                Image::drawMultiLine();
                abort(0);
            } else {
                abort(1);
            }
            break;
        }
        default:
            printf("too many steps\n");
            abort(1);
            break;
    }
}

void Tools::multiTool(void* data, ImageViewer* iv) {
    if (toolID == 0) {
        toolID = *((char*) data);
        if (toolID == RESIZETOOL) {
            resizeTool(iv);
        }
    } else {
        step++;
        switch (toolID) {
            case INVERTTOOL:
                invertTool(data);
                break;
            case BNWTOOL:
                BnWTool(data, iv);
                break;
            case RESIZETOOL:
                resizeTool(iv);
                break;
            case LINETOOL:
                lineTool(data, iv);
                break;
            case NOTOOL:
                step = 0;
                printf("triggered without tool\n");
                break;
            default:
                step = 0;
                printf("tool id error!\n");
                break;
        }
    }
}
