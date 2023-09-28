#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../viscomp-lab1/Util.h"


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

void filter_all_average(int width, int height, int ntimes, int dimension, byte* source) {
    int dimstep = (dimension - 1) / 2;
    byte* bytepadded = (byte*) calloc((width + dimstep * 2) * (height + dimstep * 2), sizeof(byte));

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

int main(int argc, char* argv[]) {
    FILE* ifp, * ofp;
    byte* bytemap;
    char* filter_type;
    int version, rows, cols, ntimes, dimension, maxval = 255;

    /* Arguments */
    if (argc != 6) {
        printf("\nUsage: %s file_in file_out \n\n", argv[0]);
        exit(0);
    }

    filter_type = argv[3];

    ntimes = atoi(argv[4]);

    dimension = atoi(argv[5]);
    if (dimension < 0 || dimension % 2 == 0) {
        printf("Filter dimension should be a positive odd number\n");
        exit(1);
    }

    /* Opening input file */
    ifp = fopen(argv[1], "r");
    if (ifp == NULL) {
        printf("Error opening file %s\n", argv[1]);
        exit(1);
    }

    /*  Magic number reading */
    getc(ifp);
    version = getc(ifp);
    if (version == EOF) pm_erreur("EOF / read error / magic number");
    if (version != '5') pm_erreur("Wrong file type");

    /* Reading image dimensions */
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);

    /* Memory allocation  */
    bytemap = (byte *) malloc(cols * rows * sizeof(byte));

    /* Reading */
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            bytemap[i * cols + j] = pm_getrawbyte(ifp);

    /* Closing input file */
    fclose(ifp);

    if (strcmp(filter_type, "gaussian") == 0) filter_all_gaussian(cols, rows, ntimes, dimension, bytemap);
    else if (strcmp(filter_type, "average") == 0) filter_all_average(cols, rows, ntimes, dimension, bytemap);
    else if (strcmp(filter_type, "mean") == 0) filter_all_mean(cols, rows, ntimes, dimension, bytemap);
    else {
        printf("Unknown filtering type: %s\n", filter_type);
        exit(1);
    }

    /* Opening output file */
    ofp = fopen(argv[2], "w");
    if (ofp == NULL) {
        printf("Error opening file %s\n", argv[2]);
        exit(1);
    }

    /* Writing */
    fprintf(ofp, "P5\n%d %d\n%d\n", cols, rows, maxval);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols ; j++)
            fprintf(ofp, "%c",bytemap[i * cols + j]);

    /* Closing output file */
    fclose(ofp);

    return 0;
}
