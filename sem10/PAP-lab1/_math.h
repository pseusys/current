#ifndef MATH_H
#define MATH_H

#include <stdlib.h>
#include "_utils.h"

void add_vectors1 (vector X, vector Y, vector Z, const size_t size);
void add_vectors2 (vector X, vector Y, vector Z, const size_t size);

double dot1 (vector X, vector Y, const size_t size);
double dot2 (vector X, vector Y, const size_t size);
double dot3 (vector X, vector Y, const size_t size);

void mult_mat_vect0 (matrix M, vector b, double *c, size_t datasize);
void mult_mat_vect1 (matrix M, vector b, vector c, size_t datasize);
void mult_mat_vect2 (matrix M, vector b, vector c, size_t datasize);

void mult_mat_mat0 (matrix A, matrix B, matrix C, size_t datasize);
void mult_mat_mat1 (matrix A, matrix B, matrix C, size_t datasize);
void mult_mat_mat2 (matrix A, matrix B, matrix C, size_t datasize);

#endif //MATH_H
