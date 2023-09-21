#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "../libs/general.h"
#include "../libs/utils.h"


void canon(int width, int height, double *A, double *B, double *C, int rank, int processes, MPI_Datatype* extra_type) {
    if (width != height) {
        printf("This canon algorithm realization implies block width (%d) to be equal block height (%d)!\n", width, height);
        MPI_Finalize();
        exit(-1);
    }

    int tag = 0;
    int root = 0;
    int dimension = (int) sqrt(processes);
    int share = height / dimension;
    int opsize = share * share;
    double* buffer = malloc(height * sizeof(double));
    for (int i = 0; i < height; i++) buffer[i] = 0;

    // PRESKEW:

    double* A_preserve, *B_preserve, * A_copy, * B_copy;
    if (rank == 0) {
        A_preserve = createMatrixCopy(width, height, A);
        B_preserve = createMatrixCopy(width, height, B);
        A_copy = createMatrixCopy(width, height, A);
        B_copy = createMatrixCopy(width, height, B);

        printf("Matrix A:\n");
        printMatrix(width, height, A_copy);
        printf("Matrix B:\n");
        printMatrix(width, height, B_copy);

        for (int i = 0; i < height; i++) {
            double diagonal = A_copy[i * width + i];
            for (int j = i; j > 0; j--) A_copy[i * width + j] = A_copy[i * width + j - 1];
            A_copy[i * width] = diagonal;
        }

        for (int i = 0; i < width; i++) {
            double diagonal = B_copy[i * height + i];
            for (int j = i; j > 0; j--) B_copy[i * height + j] = B_copy[i * height + j - 1];
            B_copy[i * height] = diagonal;
        }

        printf("Matrix A:\n");
        printMatrix(width, height, A_copy);
        printf("Matrix B:\n");
        printMatrix(width, height, B_copy);
        printf("\n\n\n");
    }

    // SCATTER:

    int counts[processes];
    for (int i = 0; i < processes; i++) counts[i] = 1;
    int displs[processes];
    for (int i = 0; i < processes; i++) displs[i] = i / dimension * width + i % dimension;

    MPI_Scatterv(A_copy, counts, displs, extra_type[0], A, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Scatterv(B_copy, counts, displs, extra_type[0], B, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
    if (rank == 0) {
        free(A_copy);
        free(B_copy);
    }

    // ALGORITHM:

    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, rank / dimension, rank, &row_comm);

    MPI_Request requests[4];
    for (int i = 0; i <= width; i++) {
        if (rank == 0) {
            printf("Round %d:\n", i);
            printf("Matrix A:\n");
            printMatrix(share, share, A);
            printf("Matrix B:\n");
            printMatrix(share, share, B);
            printf("Matrix C:\n");
            printMatrix(share, share, C);
        }

        int previous = (rank + dimension) % processes;
        int next = rank - dimension >= 0 ? rank - dimension : processes - dimension + rank % dimension;
        int right = (rank + 1) % processes;
        int left = (rank - 1) >= 0 ? rank - 1 : processes - 1;
        MPI_Isend(&A[share - 1], 1, extra_type[1], right, tag, MPI_COMM_WORLD, &requests[0]);
        MPI_Isend(&B[(share - 1) * share], share, MPI_DOUBLE, next, tag, MPI_COMM_WORLD, &requests[1]);

        for (int j = 0; j < opsize; j++) {
            int row = (rank / dimension) * share + j / share;
            int col = (row + i) % share;
            int idx_row = row / share;
            int idx_col = ((row + i) / share) % dimension;
            int idx = idx_row * dimension + idx_col;
            if (j / share == row % share && j % share == col) {
                //printf("Round %d, rank %d will receive %d from %d: %.1lf\n", i, rank, row, idx, A[j]);
                buffer[row] = A[j];
                MPI_Bcast(&buffer[row], 1, MPI_DOUBLE, idx_col, row_comm);
            }
        }

        //MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            //printf("Buffer:\n");
            //printMatrix(height, 1, buffer);
        }

        for (int j = 0; j < opsize; j++) {
            int row = (rank / dimension) * share + j / share;
            if (rank == 0) {
                printf("C[%d] = C[%d] + buffer[%d] * B[%d] = %.1lf + %.1lf * %.1lf = %.1lf\n", j, j, row, j, C[j], buffer[row], B[j], C[j] + buffer[row] * B[j]);
            }
            C[j] += buffer[row] * B[j];
        }

        MPI_Waitall(2, &requests[0], MPI_STATUSES_IGNORE);
        for (int j = share - 2; j >= 0; j--)
            for (int k = 0; k < share; k++)
                A[k * share + j + 1] = A[k * share + j];
        for (int j = share - 1; j > 0; j--)
            for (int k = 0; k < share; k++)
                B[j * share + k] = B[(j - 1) * share + k];
        MPI_Irecv(A, 1, extra_type[1], left, tag, MPI_COMM_WORLD, &requests[2]);
        MPI_Irecv(B, share, MPI_DOUBLE, previous, tag, MPI_COMM_WORLD, &requests[3]);
        MPI_Waitall(2, &requests[2], MPI_STATUSES_IGNORE);

        if (rank == 0) {
            printf("\n\n\n");
        }
    }

    // GATHER:

    MPI_Gatherv(C, opsize, MPI_DOUBLE, C, counts, displs, extra_type[0], root, MPI_COMM_WORLD);
    free(buffer);

    if (rank == 0) {
        copyMatrix(width, height, A_preserve, A);
        copyMatrix(width, height, B_preserve, B);
        free(A_preserve);
        free(B_preserve);
    }
}

int read_and_assert_matrix_size_impl(int argc, char* argv[], int processes) {
    int mat_size;
    if (argc != 2) {
        printf("Usage: %s matrix_size\n", argv[0]);
        MPI_Finalize();
        exit(-1);
    } else mat_size = atoi(argv[1]);

    double root = sqrt(processes);
    if (root * root != processes) {
        printf("Square root of process number (%d) is not an integer (%lf)!\n", processes, root);
        MPI_Finalize();
        exit(-1);
    } else if (mat_size % (int) root != 0) {
        printf("Matrix of size %d can't be splitted between %d processes!\n", mat_size, processes);
        MPI_Finalize();
        exit(-1);
    }
    return mat_size;
}

void allocate_matrixes_impl(int my_rank, int mat_size, int processes, double **A, double **B, double **C) {
    int share = mat_size / (int) sqrt(processes);
    if (my_rank == 0) {
        *A = allocMatrix(mat_size, mat_size);
        *B = allocMatrix(mat_size, mat_size);
        *C = allocMatrix(mat_size, mat_size);
    } else {
        *A = allocMatrix(share, share);
        *B = allocMatrix(share, share);
        *C = allocMatrix(share, share);
    }
}

void initialize_matrixes_impl(int my_rank, int mat_size, int processes, double *A, double *B, double *C) {
    int share = mat_size / (int) sqrt(processes);
    if (my_rank == 0) {
        initMatrix(mat_size, mat_size, A);
        initMatrix(mat_size, mat_size, B);
        initMatrixZero(mat_size, mat_size, C);
    } else initMatrixZero(share, share, C);
}

void on_start_impl(int mat_size, int processes, MPI_Datatype* extra_type) {
    int share = mat_size / (int) sqrt(processes);
    MPI_Type_vector(share, share, mat_size, MPI_DOUBLE, &extra_type[0]);
    MPI_Type_create_resized(extra_type[0], 0, share * sizeof(double), &extra_type[0]);
	MPI_Type_commit(&extra_type[0]);
    MPI_Type_vector(share, 1, share, MPI_DOUBLE, &extra_type[1]);
    MPI_Type_commit(&extra_type[1]);
}

void on_end_impl(MPI_Datatype* extra_type) {
    MPI_Type_free(&extra_type[0]);
    MPI_Type_free(&extra_type[1]);
}

int main(int argc, char* argv[]) {
    eval_config config = {
        .eval = canon,
        .rams = read_and_assert_matrix_size_impl,
        .am = allocate_matrixes_impl,
        .im = initialize_matrixes_impl,
        .start = on_start_impl,
        .end = on_end_impl
    };
    return eval(argc, argv, "canon", &config);
}
