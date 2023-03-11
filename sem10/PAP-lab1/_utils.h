#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#define NBEXPERIMENTS 22

typedef double *vector;
typedef double *matrix;

long long unsigned int average (long long unsigned int *exps);
void init_vector (vector *X, const size_t size, const double val);
void free_vector (vector X);
void init_matrix (matrix *X, const size_t size, const double val);
void free_matrix (matrix X);
void print_vectors (vector X, vector Y, const size_t size);

#endif //UTILS_H
