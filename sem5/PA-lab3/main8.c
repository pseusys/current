#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define array_upper_limit 1000000.0


void populate_integer_array(int* arr, int size, int rank) {
    srand(time(NULL) + rank);
    for (int i = 0; i < size; i++) arr[i] = (int) ((array_upper_limit / RAND_MAX) * rand());
}

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int k = ProcNum + 5;
    double start_time = MPI_Wtime();

    int* containment = malloc(k * sizeof(int));
    populate_integer_array(containment, k, ProcRank);

    int* answers = malloc(k * sizeof(int));
    MPI_Reduce(containment, answers, k, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (ProcRank == 0) {
        for (int i = 0; i < k; ++i)
            printf("Root process: minimum for index %d is %d\n", i, answers[i]);
        printf("It took %lf sec\n", MPI_Wtime() - start_time);
    }

    free(containment);
    free(answers);
    MPI_Finalize();
    return 0;
}
