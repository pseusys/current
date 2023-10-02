#include <stdlib.h>

#include "average.h"
#include "padding.h"


int average_comparator(const void* a, const void* b) {
   return *((byte*) a) - *((byte*) b);
}

byte filter_one_average(int x, int y, int width, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;
    long long int average_sum = dimension * dimension;
    byte* data = (byte*) calloc(average_sum, sizeof(byte));

    int index = 0;
    for (int i = -dimstep; i <= dimstep; i++)
        for (int j = -dimstep; j <= dimstep; j++) {
            data[index] = source[(x + i) * width + y + i];
            index++;
        }

    qsort(data, average_sum, sizeof(byte), average_comparator);
    return data[average_sum / 2];
}

void filter_all_average(char* padding_type, int maxval, int width, int height, int ntimes, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;
    byte* bytepadded = get_padded(padding_type, (width + dimstep * 2) * (height + dimstep * 2), maxval);

    for (int i = 0; i < ntimes; i++) {
        for (int i = dimstep; i < height + dimstep; i++)
            for (int j = dimstep; j < width + dimstep; j++)
                bytepadded[i * width + j] = source[(i - dimstep) * width + j - dimstep];
        for (int i = dimstep; i < height + dimstep; i++)
            for (int j = dimstep; j < width + dimstep; j++)
                source[(i - dimstep) * width + j - dimstep] = filter_one_average(i, j, width, dimension, bytepadded);
    }

    free(bytepadded);
}
