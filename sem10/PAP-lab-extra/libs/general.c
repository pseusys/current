#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "utils.h"


int read_and_assert_matrix_size(int argc, char* argv[], int processes) {
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

void allocate_matrixes(int my_rank, int mat_size, int share, double **A, double **B, double **C) {
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

void initialize_matrixes(int my_rank, int mat_size, int share, double *A, double *B, double *C) {
    if (my_rank == 0) {
        initMatrix(mat_size, mat_size, A);
        initMatrix(mat_size, mat_size, B);
        initMatrixZero(mat_size, mat_size, C);
    } else initMatrixZero(mat_size, share, C);
}

void print_matrixes_error(int mat_size, double *A, double *B, double *C, double *C_check) {
    printf("Matrix A:\n");
    printMatrix(mat_size, mat_size, A);
    printf("\nMatrix B:\n");
    printMatrix(mat_size, mat_size, B);
    printf("\nResult matrix:\n");
    printMatrix(mat_size, mat_size, C);
    printf("\nExpected matrix:\n");
    printMatrix(mat_size, mat_size, C_check);
}

int eval(int argc, char* argv[], char* eval_name, evaluator evaluator) {
    double *A, *B, *C;
    double *A_check, *B_check, *C_check;

    int my_rank;
    int w_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &w_size);

    unsigned int mat_size = read_and_assert_matrix_size(argc, argv, w_size);
    unsigned int share = mat_size / w_size;

    if (my_rank == 0) printf("Test with a matrix of size %u x %u\n", mat_size, mat_size);
    allocate_matrixes(my_rank, mat_size, share, &A, &B, &C);

    if (perf_eval) {
        for (int exp = 0; exp < EXPERIMENTS; exp++) {
            double start;
            initialize_matrixes(my_rank, mat_size, share, A, B, C);
            if (my_rank == 0) start = MPI_Wtime();
            evaluator(mat_size, mat_size, A, B, C, my_rank, w_size);
            if (my_rank == 0) experiments[exp] = MPI_Wtime() - start;
        }

        if (my_rank == 0) {
            double average = average_time();  
            printf ("\nMultiplication time (%s) \t\t\t%.3lf seconds\n\n", eval_name, average);
        }
    }

    if (check_correctness) {
        initialize_matrixes(my_rank, mat_size, share, A, B, C);
        if (my_rank == 0) {
            A_check = createMatrixCopy(mat_size, mat_size, A);
            B_check = createMatrixCopy(mat_size, mat_size, B);
            C_check = createMatrixCopy(mat_size, mat_size, C);
        }

        evaluator(mat_size, mat_size, A, B, C, my_rank, w_size);
        if (my_rank == 0) {
            matrixMultiplicationReference(mat_size, mat_size, A_check, B_check, C_check);
            if (checkMatricesEquality(mat_size, mat_size, C, C_check)) printf("\tCORRECT matrix multiplication result!\n");
            else {
                printf("\tFAILED matrix multiplication!!!\n");
                print_matrixes_error(mat_size, A, B, C, C_check);
            }
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
