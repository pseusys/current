#include <stdio.h>
#include <mpi.h>

#include <stdlib.h>

#include "src/utils.h"


/* a matrix multiplication without locality */
void sequentialMatrixMultiplication(int width, int height, double *A, double *B, double *C) {
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            for (int k = 0; k < width; k++)
                C[i * width + j] += A[i * width + k] * B[k * height + j];
}


int main(int argc, char *argv[]) {
    double *A, *B, *C;
    double *A_check, *B_check, *C_check;

    unsigned int mat_size = 0;

    int my_rank;
    int w_size;

    double start = 0, av = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &w_size);

    if (argc != 2) {
        printf("Usage: %s matrix_size\n", argv[0]);
        MPI_Finalize();
        return 0;
    } else mat_size = atoi(argv[1]);
    
    if (my_rank == 0) {
        printf("Test with a matrix of size %u x %u\n", mat_size, mat_size);
        A = allocMatrix(mat_size, mat_size);
        B = allocMatrix(mat_size, mat_size);
        C = allocMatrix(mat_size, mat_size);
    }

    if (perf_eval) {
        for (int exp = 0 ; exp < EXPERIMENTS; exp++) {
            if (my_rank == 0) {
                initMatrix(mat_size, mat_size, A);
                initMatrix(mat_size, mat_size, B);
                initMatrixZero(mat_size, mat_size, C);
                start = MPI_Wtime();
                matrixMultiplicationReference(mat_size, mat_size, A, B, C);
                experiments[exp] = MPI_Wtime() - start;
            }
        }

        if (my_rank == 0) {
            av = average_time();
            printf ("\nReference time \t\t\t\t%.3lf seconds\n\n", av) ;
        }

        for (int exp = 0; exp < EXPERIMENTS; exp++) {
            if (my_rank == 0) {
                initMatrix(mat_size, mat_size, A);
                initMatrix(mat_size, mat_size, B);
                initMatrixZero(mat_size, mat_size, C);
                start = MPI_Wtime();
                sequentialMatrixMultiplication(mat_size, mat_size, A, B, C);
                experiments[exp] = MPI_Wtime() - start;
            }
        }

        if (my_rank == 0) {
            av = average_time();  
            printf ("\nSequential time \t\t\t%.3lf seconds\n\n", av);
        }
    }

    if (check_correctness) {
        /* running my sequential implementation of the matrix multiplication */
        if (my_rank == 0) {
            initMatrix(mat_size, mat_size, A);
            initMatrix(mat_size, mat_size, B);
            initMatrixZero(mat_size, mat_size, C);
            A_check = createMatrixCopy(mat_size, mat_size, A);
            B_check = createMatrixCopy(mat_size, mat_size, B);
            C_check = createMatrixCopy(mat_size, mat_size, C);
        }

        /* check for correctness */
        if (my_rank == 0) {
            sequentialMatrixMultiplication(mat_size, mat_size, A, B, C);
            matrixMultiplicationReference(mat_size, mat_size, A_check, B_check, C_check);
            if (checkMatricesEquality(mat_size, mat_size, C, C_check)) printf("\tCORRECT matrix multiplication result!\n");
            else printf("\tFAILED matrix multiplication!!!\n");
            printf("Matrix A:\n");
            printMatrix(mat_size, mat_size, A);
            printf("\nMatrix B:\n");
            printMatrix(mat_size, mat_size, B);
            printf("\nResult matrix:\n");
            printMatrix(mat_size, mat_size, C);
            printf("\nExpected matrix:\n");
            printMatrix(mat_size, mat_size, C_check);
            free(A_check);
            free(B_check);
            free(C_check);
        }
    }

    if (my_rank == 0) {
        free(A);
        free(B);
        free(C);
    }

    MPI_Finalize();
    return 0;
}
