#include <math.h>
#include <stdlib.h>

#include "gaussian.h"


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

void filter_all_gaussian(int width, int height, int ntimes, int dimension, byte* source) {
    long long int* matrix = (long long int*) malloc(dimension * dimension * sizeof(long long int));
    long long int gaussian_sum = gaussian(dimension, matrix);

    int dimstep = (dimension - 1) / 2;
    byte* bytepadded = (byte*) calloc((width + dimstep * 2) * (height + dimstep * 2), sizeof(byte));

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
