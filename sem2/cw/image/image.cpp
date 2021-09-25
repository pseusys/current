#include <tools.h>
#include "image.h"

BitMap* Image::bitMap;
char* Image::name;
ImageViewer* Image::root;

const int BI_BITFIELDS = 3;
const int STANDART = 0;

int percent(int num, int perc) {
    int inter = num * perc;
    return inter / 100;
}

void readByteTrash(FILE* file, int quant, byte** array) { // function for reading information that is not necessary for my app
    *array = (byte*) malloc((size_t) quant * sizeof(byte));
    fread(*array, sizeof(byte), quant * sizeof(byte), file);
}

Pixel setColor(unsigned char r, unsigned char g, unsigned char b) {
    Pixel fin;
    fin.b = b;
    fin.g = g;
    fin.r = r;
    return fin;
}

void drawLine(int x1, int y1, int x2, int y2, Pixel* color) {
    Pixel** px = Image::bitMap->pixelArray;

    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    px[x2][y2] = *color;
    while(x1 != x2 || y1 != y2) {
        px[x1][y1] = *color;
        const int error2 = error * 2;
        //
        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }
}

int getOffset(int width) {
    int offset = 4 - (width * sizeof(Pixel)) % 4;
    if (offset == 4) offset = 0;
    return offset;
}

void Image::loadImage(const char *path) {
    printf("Loading file: %s\n", path);

    auto bitMap = (BitMap*) malloc(sizeof(BitMap));
    FILE* img = fopen(path, "rb");

    fread(&(bitMap->fileHeader), sizeof(byte), sizeof(FileHeader), img); // reading file header
    fread(&(bitMap->infoHeader), sizeof(byte), sizeof(InfoHeader), img); // reading info header
    if (bitMap->infoHeader.compression != STANDART) {
        printf("Unknown compression method, I can not parse this version...\n");
        ImageViewer::isSupported = 0;
    }
    if (bitMap->infoHeader.colorsNumber != 0) {
        printf("Color palette detected - no palette parsing enabled...\n");
        ImageViewer::isSupported = 0;
    };
    root->updateActions();

    bitMap->contains.ct = bitMap->infoHeader.colorsNumber * sizeof(ColorDefinition);

    bitMap->contains.dump_ih = bitMap->infoHeader.infoHeaderSize - sizeof(InfoHeader);
    bitMap->contains.gap0 = bitMap->infoHeader.compression == BI_BITFIELDS ? 12 : 0;
    bitMap->contains.gap1 = bitMap->fileHeader.pixelArrayPosition - (sizeof(FileHeader) + sizeof(InfoHeader) +
            bitMap->contains.dump_ih + bitMap->contains.gap0 + bitMap->contains.ct);
    bitMap->contains.gap2 = bitMap->fileHeader.fileSize - (sizeof(FileHeader) + sizeof(InfoHeader) +
            bitMap->contains.dump_ih + bitMap->contains.gap0 + bitMap->contains.ct +
            bitMap->contains.gap1 + bitMap->infoHeader.pixelArraySize);

    readByteTrash(img, bitMap->contains.dump_ih, &(bitMap->unreadableHeader)); // reading what left from info header
    readByteTrash(img, bitMap->contains.gap0, &(bitMap->gap0)); // reading something before color table

    bitMap->colorTable = (ColorDefinition*) malloc(bitMap->contains.ct);
    fread(bitMap->colorTable, sizeof(byte), bitMap->contains.ct, img);

    readByteTrash(img, bitMap->contains.gap1, &(bitMap->gap1)); // reading something before pixel array

    bitMap->pixelArray = (Pixel**) malloc(bitMap->infoHeader.height * sizeof(Pixel*));
    int offset = getOffset(bitMap->infoHeader.width);
    unsigned int isArraySizeSet = bitMap->infoHeader.pixelArraySize; // if pixel array size is not set, resetting it and related gap2 value too
    for (int i = 0; i < bitMap->infoHeader.height; i++) {
        bitMap->pixelArray[i] = (Pixel*) malloc(bitMap->infoHeader.width * sizeof(Pixel) + offset);
        fread(bitMap->pixelArray[i], sizeof(byte), bitMap->infoHeader.width * sizeof(Pixel) + offset, img);
        if (!isArraySizeSet) bitMap->infoHeader.pixelArraySize += bitMap->infoHeader.width * sizeof(Pixel) + offset;
    }
    if (!isArraySizeSet) bitMap->contains.gap2 = bitMap->fileHeader.fileSize -
            (sizeof(FileHeader) + sizeof(InfoHeader) + bitMap->contains.dump_ih + bitMap->contains.gap0 +
                    bitMap->contains.ct + bitMap->contains.gap1 + bitMap->infoHeader.pixelArraySize);

    readByteTrash(img, bitMap->contains.gap2, &(bitMap->gap2)); // reading something after pixel array

    fclose(img);

    Image::bitMap = bitMap;

    Image::name = (char *) malloc(strlen(path));
    strcpy(Image::name, path);
}

void Image::flushImage(BitMap* img, const char* path) {
    printf("Flushing path: %s\n", path);

    FILE* file = fopen(path, "wb");

    fwrite(&(img->fileHeader), sizeof(byte), sizeof(FileHeader), file);
    fwrite(&(img->infoHeader), sizeof(byte), sizeof(InfoHeader), file);
    fwrite(img->unreadableHeader, sizeof(byte), img->contains.dump_ih, file);

    fwrite(img->gap0, sizeof(byte), img->contains.gap0, file);
    fwrite(img->colorTable, sizeof(byte), img->infoHeader.colorsNumber * sizeof(ColorDefinition), file);
    fwrite(img->gap1, sizeof(byte), img->contains.gap1, file);

    int offset = getOffset(bitMap->infoHeader.width);
    for (int i = 0; i < img->infoHeader.height; i++) {
        fwrite(img->pixelArray[i], sizeof(byte), img->infoHeader.width * sizeof(Pixel) + offset, file);
    }

    fwrite(img->gap2, sizeof(byte), img->contains.gap2, file);

    fclose(file);

    if (root != NULL) {
        root->loadFile(path);
    }
}

void Image::bnw() {
    int w = Image::bitMap->infoHeader.width;
    int h = Image::bitMap->infoHeader.height;

    for (int i = (h-Tools::lowerY); i < (h-Tools::upperY); i++) {
        for (int j = Tools::upperX; j < Tools::lowerX; j++) {
            Pixel* px = &(Image::bitMap->pixelArray[i][j]);

            unsigned char mid;
            if (Tools::BnWAlgorythm == 0) {
                mid = (unsigned char) ((px->r + px->g + px->b) / 3);
            } else {
                mid = (unsigned char) ((((px->r + px->g + px->b) / 3) > 128) ? 255 : 0);
            }

            px->r = px->g = px->b = mid;
        }
    }

    flushImage(Image::bitMap, Image::name);
}

void Image::drawMultiLine() {
    int y1 = Image::bitMap->infoHeader.height-Tools::upperY;
    int y2 = Image::bitMap->infoHeader.height-Tools::lowerY;
    int x1 = Tools::upperX;
    int x2 = Tools::lowerX;

    int h = Tools::thickness - 1;
    for(int i = -h; i <= h; i++)
        for (int j = -h; j <= h; j++)
            if ((y1+i > 0) && (x1+j > 0) && (y2+i > 0) && (x2+j > 0) &&
                    (x1+i < Image::bitMap->infoHeader.width) && (y1+j < Image::bitMap->infoHeader.height) && (x1+i < Image::bitMap->infoHeader.width) && (x2+j < Image::bitMap->infoHeader.width)) {
                printf("Line written: %i, %i -> %i, %i\n", y1, x1, y2, x2);
                drawLine(y1 + i, x1 + j, y2 + i, x2 + j, Tools::color);
            }

    flushImage(Image::bitMap, Image::name);
}

void Image::invertColors() {
    int w = Image::bitMap->infoHeader.width;
    int h = Image::bitMap->infoHeader.height;

    for (int i = (h-Tools::lowerY); i < (h-Tools::upperY); i++) {
        for (int j = Tools::upperX; j < Tools::lowerX; j++) {
            Pixel* px = &(Image::bitMap->pixelArray[i][j]);
            px->r = (unsigned char) 255 - px->r;
            px->g = (unsigned char) 255 - px->g;
            px->b = (unsigned char) 255 - px->b;
        }
    }

    flushImage(Image::bitMap, Image::name);
}

void Image::resize() {
    unsigned int newWidth = Image::bitMap->infoHeader.width + percent(Image::bitMap->infoHeader.width, Tools::scalePercent);
    unsigned int newHeight = Image::bitMap->infoHeader.height + percent(Image::bitMap->infoHeader.height, Tools::scalePercent);

    Pixel** pixelArray;
    pixelArray = (Pixel**) malloc(newHeight * sizeof(Pixel*));
    int newOffset = getOffset(newWidth);
    int oldArraySize = bitMap->infoHeader.pixelArraySize;
    bitMap->infoHeader.pixelArraySize = 0;
    for (int i = 0; i < newHeight; i++) {
        pixelArray[i] = (Pixel*) malloc(newWidth * sizeof(Pixel) + newOffset);
        bitMap->infoHeader.pixelArraySize += newWidth * sizeof(Pixel) + newOffset;
    }
    int fileDiff = bitMap->infoHeader.pixelArraySize - oldArraySize;

    unsigned int hoff = (unsigned int) abs(percent(Image::bitMap->infoHeader.height, Tools::scalePercent));
    unsigned int woff = (unsigned int) abs(percent(Image::bitMap->infoHeader.width, Tools::scalePercent));

    if (Tools::scalePercent < 0) {
        if (Tools::centerPosition == -2) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    pixelArray[i][j] = bitMap->pixelArray[i+hoff][j+woff];
                }
            }
        } else if (Tools::centerPosition == -1) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    pixelArray[i][j] = bitMap->pixelArray[i+hoff][j];
                }
            }
        } else if (Tools::centerPosition == 0) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    pixelArray[i][j] = bitMap->pixelArray[i+hoff/2][j+woff/2];
                }
            }
        } else if (Tools::centerPosition == 1) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    pixelArray[i][j] = bitMap->pixelArray[i][j];
                }
            }
        } else if (Tools::centerPosition == 2) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    pixelArray[i][j] = bitMap->pixelArray[i][j+woff];
                }
            }
        }
    } else {
        if (Tools::centerPosition == -2) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    if ((i < bitMap->infoHeader.height) && (j < bitMap->infoHeader.width)) {
                        pixelArray[i+hoff][j+woff] = bitMap->pixelArray[i][j];
                    } else {
                        pixelArray[newHeight-i-1][newWidth-j-1] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                    }
                }
            }
        } else if (Tools::centerPosition == -1) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    if ((i < bitMap->infoHeader.height) && (j < bitMap->infoHeader.width)) {
                        pixelArray[i+hoff][j] = bitMap->pixelArray[i][j];
                    } else {
                        pixelArray[newHeight-i-1][j] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                    }
                }
            }
        } else if (Tools::centerPosition == 0) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    if ((i < bitMap->infoHeader.height) && (j < bitMap->infoHeader.width)) {
                        pixelArray[i+hoff/2][j+woff/2] = bitMap->pixelArray[i][j];
                    } else {
                        if ((i > bitMap->infoHeader.height) && (i < bitMap->infoHeader.height + hoff/2)) {
                            pixelArray[i-bitMap->infoHeader.height][j] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                        } else if (i > bitMap->infoHeader.height + hoff/2) {
                            pixelArray[i][j] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                        }
                        if ((j > bitMap->infoHeader.width) && (j < bitMap->infoHeader.width + woff/2)) {
                            pixelArray[i][j-bitMap->infoHeader.width] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                        } else if (j > bitMap->infoHeader.width + woff/2) {
                            pixelArray[i][j] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                        }
                    }
                }
            }
        } else if (Tools::centerPosition == 1) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    if ((i < bitMap->infoHeader.height) && (j < bitMap->infoHeader.width)) {
                        pixelArray[i][j] = bitMap->pixelArray[i][j];
                    } else {
                        pixelArray[i][j] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                    }
                }
            }
        } else if (Tools::centerPosition == 2) {
            for (int i = 0; i < newHeight; i++) {
                for (int j = 0; j < newWidth; j++) {
                    if ((i < bitMap->infoHeader.height) && (j < bitMap->infoHeader.width)) {
                        pixelArray[i][j+woff] = bitMap->pixelArray[i][j];
                    } else {
                        pixelArray[i][newWidth-j-1] = setColor(Tools::color->r, Tools::color->g, Tools::color->b);
                    }
                }
            }
        }
    }

    Image::bitMap->pixelArray = pixelArray;

    Image::bitMap->fileHeader.fileSize += fileDiff;
    Image::bitMap->infoHeader.width = newWidth;
    Image::bitMap->infoHeader.height = newHeight;

    flushImage(Image::bitMap, Image::name);
}



void Image::rotate() {
    Pixel** pixelArray;
    pixelArray = (Pixel**) malloc(Image::bitMap->infoHeader.height * sizeof(Pixel*));
    int newOffset = getOffset(Image::bitMap->infoHeader.width);
    for (int i = 0; i < Image::bitMap->infoHeader.height; i++) {
        pixelArray[i] = (Pixel*) malloc(Image::bitMap->infoHeader.width * sizeof(Pixel) + newOffset);
    }

    bool ok;
    int scalePercent = QInputDialog::getInt(root, Tools::tr("Select percent:"), Tools::tr("Resize in percent:"), 0, -100, 100, 1, &ok);
    int degree;
    if (ok) {
        degree = scalePercent % 180;
    } else {
        degree = 0;
    }


    double radians = (degree * M_PI) / 180;
    double sinf = sin(radians);
    double cosf = cos(radians);

    double x0 = 0.5 * (Image::bitMap->infoHeader.width - 1);     // point to rotate about
    double y0 = 0.5 * (Image::bitMap->infoHeader.height - 1);      // center of image

    // rotation
    for (int x = 0; x < Image::bitMap->infoHeader.width; x++) {
        for (int y = 0; y < Image::bitMap->infoHeader.height; y++) {
            long double a = x - x0;
            long double b = y - y0;
            int xx = (int) (+a * cosf - b * sinf + x0);
            int yy = (int) (+a * sinf + b * cosf + y0);

            printf("(%i, %i) ", xx, yy);

            if (xx >= 0 && xx < Image::bitMap->infoHeader.width && yy >= 0 && yy < Image::bitMap->infoHeader.height) {
                printf("%i, %i -> %i, %i\n", x, y, xx, yy);
                pixelArray[y][x] = Image::bitMap->pixelArray[yy][xx];
            }
        }
    }

    Image::bitMap->pixelArray = pixelArray;
    flushImage(Image::bitMap, Image::name);
}


