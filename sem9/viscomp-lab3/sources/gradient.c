#include <math.h>
#include <stdlib.h>

#include "gradient.h"
#include "padding.h"


int scharr_matrix[9] = {-3, 0, 3, -10, 0, 10, -3, 0, 3};
int scharr_weight = 16;


byte scharr_gradient_one(int x, int y, int width, byte* source) {
    int Gx = 0, Gy = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * scharr_matrix[(i + 1) * 3 + j + 1];
            Gy += source[(x + i) * width + y + i] * scharr_matrix[(j + 1) * 3 + i + 1];
        }
    return round(sqrt(Gx * Gx + Gy * Gy) / scharr_weight);
}

void scharr_gradient_all(char* padding_type, int maxval, int width, int height, byte* source) {
    byte* bytepadded = get_padded(padding_type, (width + 2) * (height + 2), maxval);
    for (int i = 1; i < height + 1; i++)
        for (int j = 1; j < width + 1; j++)
            bytepadded[i * width + j] = source[(i - 1) * width + j - 1];
    for (int i = 1; i < height + 1; i++)
        for (int j = 1; j < width + 1; j++)
            source[(i - 1) * width + j - 1] = scharr_gradient_one(i, j, width, bytepadded);
    free(bytepadded);
}


int sobel_matrix[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
int sobel_weight = 4;


byte sobel_gradient_one(int x, int y, int width, byte* source) {
    int Gx = 0, Gy = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * sobel_matrix[(i + 1) * 3 + j + 1];
            Gy += source[(x + i) * width + y + i] * sobel_matrix[(j + 1) * 3 + i + 1];
        }
    return round(sqrt(Gx * Gx + Gy * Gy) / sobel_weight);
}

void sobel_gradient_all(char* padding_type, int maxval, int width, int height, byte* source) {
    byte* bytepadded = get_padded(padding_type, (width + 2) * (height + 2), maxval);
    for (int i = 1; i < height + 1; i++)
        for (int j = 1; j < width + 1; j++)
            bytepadded[i * width + j] = source[(i - 1) * width + j - 1];
    for (int i = 1; i < height + 1; i++)
        for (int j = 1; j < width + 1; j++)
            source[(i - 1) * width + j - 1] = sobel_gradient_one(i, j, width, bytepadded);
    free(bytepadded);
}


int prewitt_matrix[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
int prewitt_weight = 4;


byte prewitt_gradient_one(int x, int y, int width, byte* source) {
    int Gx = 0, Gy = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * prewitt_matrix[(i + 1) * 3 + j + 1];
            Gy += source[(x + i) * width + y + i] * prewitt_matrix[(j + 1) * 3 + i + 1];
        }
    return round(sqrt(Gx * Gx + Gy * Gy) / prewitt_weight);
}

void prewitt_gradient_all(char* padding_type, int maxval, int width, int height, byte* source) {
    byte* bytepadded = get_padded(padding_type, (width + 2) * (height + 2), maxval);
    for (int i = 1; i < height + 1; i++)
        for (int j = 1; j < width + 1; j++)
            bytepadded[i * width + j] = source[(i - 1) * width + j - 1];
    for (int i = 1; i < height + 1; i++)
        for (int j = 1; j < width + 1; j++)
            source[(i - 1) * width + j - 1] = prewitt_gradient_one(i, j, width, bytepadded);
    free(bytepadded);
}


int roberts_matrix[4] = {1, 0, 0, -1};


byte roberts_gradient_one(int x, int y, int width, byte* source) {
    int Gx = 0, Gy = 0;
    for (int i = 0; i <= 1; i++)
        for (int j = 0; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * roberts_matrix[i * 2 + j];
            Gy += source[(x + i) * width + y + i] * roberts_matrix[j * 2 + i];
        }
    return round(sqrt(Gx * Gx + Gy * Gy));
}

void roberts_gradient_all(char* padding_type, int maxval, int width, int height, byte* source) {
    byte* bytepadded = get_padded(padding_type, (width + 1) * (height + 1), maxval);
    for (int i = 0; i <= height; i++)
        for (int j = 0; j <= width; j++)
            bytepadded[i * width + j] = source[i * width + j];
    for (int i = 0; i <= height; i++)
        for (int j = 0; j <= width; j++)
            source[i * width + j] = roberts_gradient_one(i, j, width, bytepadded);
    free(bytepadded);
}
