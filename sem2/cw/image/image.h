#ifndef COURSE_WORK_IMAGE_H
#define COURSE_WORK_IMAGE_H

class ImageViewer;

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include "window.h"

#define byte char

#pragma pack(push, 1)
typedef struct {
    unsigned char  marker1;
    unsigned char  marker2;
    unsigned int   fileSize;
    uint16_t       reserved1;
    uint16_t       reserved2;
    unsigned int   pixelArrayPosition;
} FileHeader;

typedef struct {
    unsigned int   infoHeaderSize;
    unsigned int   width;
    unsigned int   height;
    uint16_t       planes;
    uint16_t       bitsPerPixel;
    unsigned int   compression;
    unsigned int   pixelArraySize;
    int            horizontalPixelPerMeter;
    int            verticalPixelPerMeter;
    unsigned int   colorsNumber;
    unsigned int   importantColorsNumber;
} InfoHeader;

typedef struct {
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
    unsigned char  a;
} ColorDefinition;

typedef struct {
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
} Pixel;

typedef struct {
    unsigned int dump_ih;
    unsigned int gap0;
    unsigned int ct;
    unsigned int gap1;
    unsigned int gap2;
} Contains;

typedef struct {
    FileHeader fileHeader;
    InfoHeader infoHeader;
    byte* unreadableHeader;
    byte* gap0;
    ColorDefinition* colorTable;
    byte* gap1;
    Pixel** pixelArray;
    byte* gap2;
    Contains contains;
} BitMap;
#pragma pack(pop)

class Image {
public:
    static void loadImage(const char* path);
    static void flushImage(BitMap *bmp, const char *path);
    static void bnw();
    static void invertColors();
    static void drawMultiLine();
    static void resize();
    static void rotate();

    static ImageViewer* root;
    static BitMap* bitMap;
    static char* name;
};

#endif //COURSE_WORK_IMAGE_H
