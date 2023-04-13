#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "utils.h"


void matrix_multiplication_default(int width, int height, double *A, double *B, double *C) {
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            for (int k = 0; k < width; k++)
                C[i * width + j] += A[i * width + k] * B[k * height + j];
}

int read_and_assert_matrix_size_default(int argc, char* argv[], int processes) {
    int mat_size;
    if (argc != 2) {
        printf("Usage: %s matrix_size\n", argv[0]);
        MPI_Finalize();
        exit(-1);
    } else mat_size = atoi(argv[1]);

    if (mat_size % processes != 0) {
        printf("Matrix of size %d can't be splitted between %d processes!\n", mat_size, processes);
        MPI_Finalize();
        exit(-1);
    }
    return mat_size;
}

void allocate_matrixes_default(int my_rank, int mat_size, int share, double **A, double **B, double **C) {
    if (my_rank == 0) {
        *A = allocMatrix(mat_size, mat_size);
        *B = allocMatrix(mat_size, mat_size);
        *C = allocMatrix(mat_size, mat_size);
    } else {
        *A = allocMatrix(mat_size, share);
        *B = allocMatrix(mat_size, mat_size);
        *C = allocMatrix(mat_size, share);
    }
}

void initialize_matrixes_default(int my_rank, int mat_size, int share, double *A, double *B, double *C) {
    if (my_rank == 0) {
        initMatrix(mat_size, mat_size, A);
        initMatrix(mat_size, mat_size, B);
        initMatrixZero(mat_size, mat_size, C);
    } else initMatrixZero(mat_size, share, C);
}

void print_matrixes_error_default(int mat_size, double *A, double *B, double *C, double *C_check) {
    printf("Matrix A:\n");
    printMatrix(mat_size, mat_size, A);
    printf("\nMatrix B:\n");
    printMatrix(mat_size, mat_size, B);
    printf("\nResult matrix:\n");
    printMatrix(mat_size, mat_size, C);
    printf("\nExpected matrix:\n");
    printMatrix(mat_size, mat_size, C_check);
}
