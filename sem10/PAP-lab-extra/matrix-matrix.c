#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

#include <stdlib.h>

#include "src/utils.h"


void matrixMatrixMultiplication(int width, int height, double *A, double *B, double *C, int rank, int size) {
    int tag = 0;
    int root = 0;
    int share = height / size;
    int opsize = share * width;
    MPI_Scatter(A, opsize, MPI_DOUBLE, A, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Scatter(B, opsize, MPI_DOUBLE, B + opsize * rank, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);

    MPI_Request requests[2];
    int next = rank == 0 ? size - 1 : rank - 1;
    int previous = rank == size - 1 ? 0 : rank + 1;
    for (int p = 0; p < size; p++) {
        int current = (p + rank) % size;
        int recieving = (p + 1 + rank) % size;
        MPI_Isend(&B[opsize * current], opsize, MPI_DOUBLE, next, tag, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&B[opsize * recieving], opsize, MPI_DOUBLE, previous, tag, MPI_COMM_WORLD, &requests[1]);

        for (int i = 0; i < share; i++) {
            for (int j = 0; j < width; j++) {
                for (int k = current * share; k < (current + 1) * share; k++) {
                    C[i * width + j] += A[i * width + k] * B[k * height + j];
                }
            }
        }
        
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    }

    MPI_Gather(C, opsize, MPI_DOUBLE, C, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
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

    if (mat_size % w_size != 0) {
        printf("Matrix of size %d can't be splitted between %d processes!\n", mat_size, w_size);
        MPI_Finalize();
        return 0;
    }

    int share = mat_size / w_size;

    if (my_rank == 0) {
        printf("Test with a matrix of size %u x %u\n", mat_size, mat_size);
        A = allocMatrix(mat_size, mat_size);
        B = allocMatrix(mat_size, mat_size);
        C = allocMatrix(mat_size, mat_size);
    } else {
        A = allocMatrix(mat_size, share);
        B = allocMatrix(mat_size, mat_size);
        C = allocMatrix(mat_size, share);
    }

    if (perf_eval) {
        for (int exp = 0; exp < EXPERIMENTS; exp++) {
            if (my_rank == 0) {
                initMatrix(mat_size, mat_size, A);
                initMatrix(mat_size, mat_size, B);
                initMatrixZero(mat_size, mat_size, C);
                start = MPI_Wtime();
            } else initMatrixZero(mat_size, share, C);
            matrixMatrixMultiplication(mat_size, mat_size, A, B, C, my_rank, w_size);
            if (my_rank == 0) experiments[exp] = MPI_Wtime() - start;
        }

        if (my_rank == 0) {
            av = average_time();  
            printf ("\nMatrix matrix multiplication time \t\t\t%.3lf seconds\n\n", av);
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
        matrixMatrixMultiplication(mat_size, mat_size, A, B, C, my_rank, w_size);
        if (my_rank == 0) {
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

    free(A);
    free(B);
    free(C);

    MPI_Finalize();
    return 0;
}
