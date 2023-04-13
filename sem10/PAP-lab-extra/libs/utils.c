#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "utils.h"

#ifdef PERF_EVAL
int perf_eval = 1;
#else
int perf_eval = 0;
#endif

#ifdef CHECK_CORRECTNESS
int check_correctness = 1;
#else
int check_correctness = 0;
#endif

#ifdef RINIT
int rinit = 1;
#else
int rinit = 0;
#endif


double experiments[EXPERIMENTS];

/* return the average time in cycles over the values stored in experiments vector */
double average_time(void) {
    double s = 0;
    for (int i = 0; i < EXPERIMENTS; i++) s = s + experiments[i];
    return s / EXPERIMENTS;
}


/* allocates a w x h matrix */
double* allocMatrix(int width, int height) {
    return (double*) malloc(sizeof(double) * width * height);
}

/* initializes the content of matrix A */
void initMatrix(int width, int height, double *A) {
    int mean = (width + height) / 2;
    if (rinit) srand(time(NULL)); // TODO: CHANGE
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            A[i * width + j] = rinit ? rand() % mean : 2;
}

/* fills matrix A with 0*/
void initMatrixZero(int width, int height, double *A) {
    memset(A, 0, sizeof(double) * width * height);
}

/* create a copy of matrix A */
double* createMatrixCopy(int width, int height, double *A) {
    double* mat = (double*) malloc(sizeof(double) * width * height);
    memcpy(mat, A, sizeof(double) * width * height);
    return mat;
}



/* return 1 if V1 and V2 are equal */
int checkMatricesEquality(int width, int height, double *V1, double *V2) {
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (V1[i * width + j] != V2[i * width + j]) return 0;
    return 1;
}

/* display the content of a matrix */
void printMatrix(int width, int height, double *A) {
    printf("Matrix content:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++)
            printf("\t%.1lf", A[i * width + j]);
        printf("\n");
    }
}
