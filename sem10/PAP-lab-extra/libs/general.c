#include <stdio.h>
#include <stdlib.h>

#include "general.h"
#include "utils.h"


int eval(int argc, char* argv[], char* eval_name, eval_config* config) {
    if (!config->eval) {
        printf("Matrix evaluation function not defined!\n");
        exit(1);
    }
    if (!config->rams) config->rams = read_and_assert_matrix_size_default;
    if (!config->am) config->am = allocate_matrixes_default;
    if (!config->im) config->im = initialize_matrixes_default;
    if (!config->pme) config->pme = print_matrixes_error_default;

    double *A, *B, *C;
    double *A_check, *B_check, *C_check;

    int my_rank;
    int w_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &w_size);

    MPI_Datatype extra_type;
    unsigned int mat_size = config->rams(argc, argv, w_size);
    if (config->start) config->start(mat_size, w_size, &extra_type);

    if (my_rank == 0) printf("Test with a matrix of size %u x %u\n", mat_size, mat_size);
    config->am(my_rank, mat_size, w_size, &A, &B, &C);

    if (perf_eval) {
        for (int exp = 0; exp < EXPERIMENTS; exp++) {
            double start;
            config->im(my_rank, mat_size, w_size, A, B, C);
            if (my_rank == 0) start = MPI_Wtime();
            config->eval(mat_size, mat_size, A, B, C, my_rank, w_size, &extra_type);
            if (my_rank == 0) experiments[exp] = MPI_Wtime() - start;
        }

        if (my_rank == 0) {
            double average = average_time();  
            printf ("\nMultiplication time (%s) \t\t\t%.3lf seconds\n\n", eval_name, average);
        }
    }

    if (check_correctness) {
        config->im(my_rank, mat_size, w_size, A, B, C);
        if (my_rank == 0) {
            A_check = createMatrixCopy(mat_size, mat_size, A);
            B_check = createMatrixCopy(mat_size, mat_size, B);
            C_check = createMatrixCopy(mat_size, mat_size, C);
        }

        config->eval(mat_size, mat_size, A, B, C, my_rank, w_size, &extra_type);
        if (my_rank == 0) {
            matrix_multiplication_default(mat_size, mat_size, A_check, B_check, C_check);
            if (checkMatricesEquality(mat_size, mat_size, C, C_check)) printf("\tCORRECT matrix multiplication result!\n");
            else {
                printf("\tFAILED matrix multiplication!!!\n");
                config->pme(mat_size, A, B, C, C_check);
            }
            free(A_check);
            free(B_check);
            free(C_check);
        }
    }

    if (config->end) config->end(&extra_type);

    free(A);
    free(B);
    free(C);

    MPI_Finalize();
    return 0;
}
