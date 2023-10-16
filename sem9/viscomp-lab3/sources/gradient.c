#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gradient.h"
#include "padding.h"


int scharr_matrix[9] = {-3, 0, 3, -10, 0, 10, -3, 0, 3};
int scharr_weight = 16;

void scharr_gradient_one(int x, int y, int width, byte* source, Gradient* grad) {
    int Gx = 0, Gy = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * scharr_matrix[(i + 1) * 3 + j + 1];
            Gy += source[(x + i) * width + y + i] * scharr_matrix[(j + 1) * 3 + i + 1];
        }
    grad->magnitude = round(sqrt(Gx * Gx + Gy * Gy) / scharr_weight);
    int angle_rnd = (int) (atan2(Gy, Gx) * (180.0 / M_PI) + 180.0);
    grad->angle = ((int) round(angle_rnd / 45.0)) % 8;
}


int sobel_matrix[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
int sobel_weight = 4;

void sobel_gradient_one(int x, int y, int width, byte* source, Gradient* grad) {
    int Gx = 0, Gy = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * sobel_matrix[(i + 1) * 3 + j + 1];
            Gy += source[(x + i) * width + y + i] * sobel_matrix[(j + 1) * 3 + i + 1];
        }
    grad->magnitude = round(sqrt(Gx * Gx + Gy * Gy) / sobel_weight);
    int angle_rnd = (int) (atan2(Gy, Gx) * (180.0 / M_PI) + 180.0);
    grad->angle = ((int) round(angle_rnd / 45.0)) % 8;
}


int prewitt_matrix[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
int prewitt_weight = 4;

void prewitt_gradient_one(int x, int y, int width, byte* source, Gradient* grad) {
    int Gx = 0, Gy = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * prewitt_matrix[(i + 1) * 3 + j + 1];
            Gy += source[(x + i) * width + y + i] * prewitt_matrix[(j + 1) * 3 + i + 1];
        }
    grad->magnitude = round(sqrt(Gx * Gx + Gy * Gy) / prewitt_weight);
    int angle_rnd = (int) (atan2(Gy, Gx) * (180.0 / M_PI) + 180.0);
    grad->angle = ((int) round(angle_rnd / 45.0)) % 8;
}


int roberts_matrix[4] = {1, 0, 0, -1};

void roberts_gradient_one(int x, int y, int width, byte* source, Gradient* grad) {
    int Gx = 0, Gy = 0;
    for (int i = 0; i <= 1; i++)
        for (int j = 0; j <= 1; j++) {
            Gx += source[(x + i) * width + y + i] * roberts_matrix[i * 2 + j];
            Gy += source[(x + i) * width + y + i] * roberts_matrix[j * 2 + i];
        }
    grad->magnitude = round(sqrt(Gx * Gx + Gy * Gy));
    int angle_rnd = (int) (atan2(Gy, Gx) * (180.0 / M_PI) + 180.0);
    grad->angle = ((int) round(angle_rnd / 45.0)) % 8;
}


void (*get_gradient_one(char* gradient_type)) (int, int, int, byte*, Gradient*) {
    if (strcmp(gradient_type, "roberts") == 0) return roberts_gradient_one;
    else if (strcmp(gradient_type, "prewitt") == 0) return prewitt_gradient_one;
    else if (strcmp(gradient_type, "sobel") == 0) return sobel_gradient_one;
    else if (strcmp(gradient_type, "scharr") == 0) return scharr_gradient_one;
    else pm_erreur("Unknown gradient type");
    return NULL;
}


void gradient_all(char* gradient_type, char* padding_type, int maxval, int width, int height, byte* source) {
    void (*gradient_one)(int, int, int, byte*, Gradient*) = get_gradient_one(gradient_type);

    Gradient* grad = (Gradient*) malloc(sizeof(Gradient));
    byte* bytepadded = get_padded(padding_type, (width + 2) * (height + 2), maxval);
    for (int i = 1; i <= height; i++)
        for (int j = 1; j <= width; j++)
            bytepadded[i * width + j] = source[(i - 1) * width + j - 1];
    for (int i = 1; i <= height; i++)
        for (int j = 1; j <= width; j++) {
            gradient_one(i, j, width, bytepadded, grad);
            source[(i - 1) * width + j - 1] = grad->magnitude;
        }
    free(bytepadded);
    free(grad);
}
