#ifndef UTILS_H
#define UTILS_H

#ifndef EXPERIMENTS
#define EXPERIMENTS 5
#endif

#define MAX_TYPE_COUNT 16

extern double experiments [];

extern int perf_eval;
extern int check_correctness;
extern int rinit;

double* allocMatrix(int width, int height);

void initMatrix(int width, int height, double *A);
void initMatrixZero(int width, int height, double *A);

double* createMatrixCopy(int width, int height, double *A);
double* copyMatrix(int width, int height, double *A, double *B);

void matrixMultiplicationReference(int width, int height, double *A, double *B, double *C);

int checkMatricesEquality(int width, int height, double *V1, double *V2);

void printMatrix(int width, int height, double *A);

double average_time();

#endif // UTILS_H
