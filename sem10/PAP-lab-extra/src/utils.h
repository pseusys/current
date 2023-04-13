#ifndef UTILS_H
#define UTILS_H

#ifndef EXPERIMENTS
#define EXPERIMENTS 5
#endif

typedef void (*evaluator) (int, int, double*, double*, double*, int, int);

int eval(int argc, char* argv[], char* eval_name, evaluator evaluator);

extern double experiments [];

extern int perf_eval;
extern int check_correctness;
extern int rinit;

double* allocMatrix(int width, int height);

void initMatrix(int width, int height, double *A);
void initMatrixZero(int width, int height, double *A);

double* createMatrixCopy(int width, int height, double *A);

void matrixMultiplicationReference(int width, int height, double *A, double *B, double *C);

int checkMatricesEquality(int width, int height, double *V1, double *V2);

void printMatrix(int width, int height, double *A);

double average_time();

#endif // UTILS_H
