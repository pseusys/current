#include <math.h>
#include <stdlib.h>

#include "filter.h"
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


long long int gaussian(int dimension, long long int* matrix) {
    long long int sum = 0;
    int dimstep = (dimension - 1) / 2;
    for (int i = 0; i <= dimstep; i++) {
        for (int j = 0; j <= dimstep; j++) {
            long long int val = pow(2, j + i);
            matrix[i * dimension + j] = val;
            matrix[(i + 1) * dimension - 1 - j] = val;
            matrix[(dimension - 1 - i) * dimension + j] = val;
            matrix[(dimension - i) * dimension - 1 - j] = val;
            sum += val * (i == dimstep ? 1 : 2) * (j == dimstep ? 1 : 2);
        }
    }
    return sum;
}

long long int filter_one_gaussian(int x, int y, int width, int dimension, long long int* gaussian, byte* source) {
    int dimstep = (dimension - 1) / 2;
    long long int result = 0;
    for (int i = -dimstep; i <= dimstep; i++)
        for (int j = -dimstep; j <= dimstep; j++)
            result += gaussian[(i + dimstep) * dimension + j + dimstep] * source[(x + i) * width + y + i];
    return result;
}

void filter_all_gaussian(char* padding_type, int maxval, int width, int height, int ntimes, int dimension, byte* source) {
    long long int* matrix = (long long int*) malloc(dimension * dimension * sizeof(long long int));
    long long int gaussian_sum = gaussian(dimension, matrix);

    int dimstep = (dimension - 1) / 2;
    byte* bytepadded = get_padded(padding_type, (width + dimstep * 2) * (height + dimstep * 2), maxval);

    for (int i = 0; i < ntimes; i++) {
        for (int i = dimstep; i < height + dimstep; i++)
            for (int j = dimstep; j < width + dimstep; j++)
                bytepadded[i * width + j] = source[(i - dimstep) * width + j - dimstep];
        for (int i = dimstep; i < height + dimstep; i++)
            for (int j = dimstep; j < width + dimstep; j++)
                source[(i - dimstep) * width + j - dimstep] = round(filter_one_gaussian(i, j, width, dimension, matrix, bytepadded) / gaussian_sum);
    }

    free(bytepadded);
    free(matrix);
}


byte filter_one_mean(int x, int y, int width, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;

    long long int data_sum = 0;
    for (int i = -dimstep; i <= dimstep; i++)
        for (int j = -dimstep; j <= dimstep; j++)
            data_sum += source[(x + i) * width + y + i];

    return data_sum / (dimension * dimension);
}

void filter_all_mean(char* padding_type, int maxval, int width, int height, int ntimes, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;
    byte* bytepadded = get_padded(padding_type, (width + dimstep * 2) * (height + dimstep * 2), maxval);

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

