#ifndef PA_LAB6_MATRIX_H
#define PA_LAB6_MATRIX_H

typedef struct {
    int dimension;
    int* elements;
} matrix;

matrix* create_matrix (int d);
matrix* input_matrix ();

void print_matrix(matrix* m);

void remove_matrix (matrix* m);

#endif //PA_LAB6_MATRIX_H
