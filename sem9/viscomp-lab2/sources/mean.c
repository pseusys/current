#include <stdlib.h>

#include "mean.h"


byte filter_one_mean(int x, int y, int width, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;

    long long int data_sum = 0;
    for (int i = -dimstep; i <= dimstep; i++)
        for (int j = -dimstep; j <= dimstep; j++)
            data_sum += source[(x + i) * width + y + i];

    return data_sum / (dimension * dimension);
}

void filter_all_mean(int width, int height, int ntimes, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;
    byte* bytepadded = (byte*) calloc((width + dimstep * 2) * (height + dimstep * 2), sizeof(byte));

    for (int i = 0; i < ntimes; i++) {
        for (int i = dimstep; i < height + dimstep; i++)
            for (int j = dimstep; j < width + dimstep; j++)
                bytepadded[i * width + j] = source[(i - dimstep) * width + j - dimstep];
        for (int i = dimstep; i < height + dimstep; i++)
            for (int j = dimstep; j < width + dimstep; j++)
                source[(i - dimstep) * width + j - dimstep] = filter_one_mean(i, j, width, dimension, bytepadded);
    }

    free(bytepadded);
}
