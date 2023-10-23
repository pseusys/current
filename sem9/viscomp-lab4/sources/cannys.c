#include <stdlib.h>
#include <string.h>

#include "cannys.h"
#include "gradient.h"
#include "padding.h"


void non_max_suppress(char* gradient_type, char* padding_type, int maxval, int width, int height, byte* source) {
    void (*gradient_one)(int, int, int, byte*, Gradient*) = get_gradient_one(gradient_type);

    int more_buffer = (width + 2) * (height + 2);
    byte* bytepadded = get_padded(padding_type, more_buffer, maxval);
    Gradient* grads = (Gradient*) calloc(more_buffer, sizeof(Gradient));
    for (int i = 1; i <= height; i++)
        for (int j = 1; j <= width; j++)
            bytepadded[i * width + j] = source[(i - 1) * width + j - 1];
    for (int i = 0; i < height + 2; i++)
        for (int j = 0; j < width + 2; j++)
            gradient_one(i, j, width, bytepadded, &(grads[i * width + j]));
    for (int i = 1; i <= height; i++)
        for (int j = 1; j <= width; j++) {
            Gradient current = grads[i * width + j];
            int row = (current.angle / 3) - 1;
            int col = (current.angle % 3) - 1;
            Gradient pointed = grads[(i + col) * width + j + row];
            if (pointed.magnitude > current.magnitude) source[(i - 1) * width + j - 1] = 0;
            else source[(i - 1) * width + j - 1] = current.magnitude;
        }
    free(bytepadded);
    free(grads);
}


void hysteresis_pixel(int x, int y, int width, byte* source, byte* padded, byte threshold) {
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            byte* source_byte = &(source[(x + i) * width + y + j]);
            byte* padded_byte = &(padded[(x + i + 1) * width + y + j + 1]);
            if (i == 0 && j == 0) *source_byte = *padded_byte;
            else if (*padded_byte > threshold && *source_byte != *padded_byte)
                hysteresis_pixel(x + i, y + j, width, source, padded, threshold);
        }
}

void hysteresis_threshold(char* gradient_type, char* padding_type, int maxval, int width, int height, byte* source, byte higher, byte lower) {
    non_max_suppress(gradient_type, padding_type, maxval, width, height, source);

    byte* bytepadded = get_padded("zero", (width + 2) * (height + 2), maxval);
    for (int i = 1; i <= height; i++)
        for (int j = 1; j <= width; j++)
            bytepadded[i * width + j] = source[(i - 1) * width + j - 1];

    memset(source, 0, width * height);
    for (int i = 1; i <= height; i++)
        for (int j = 1; j <= width; j++)
            if (bytepadded[i * width + j] > higher)
                hysteresis_pixel(i - 1, j - 1, width, source, bytepadded, lower);

    free(bytepadded);
}
