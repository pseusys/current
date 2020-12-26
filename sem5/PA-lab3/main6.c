#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define array_upper_limit 1000000.0

void populate_array(int* arr, int size) {
    for (int i = 0; i < size; i++) arr[i] = (int) ((array_upper_limit / RAND_MAX) * rand());
}

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand(time(NULL) + ProcRank);

    int k = ProcNum * 3;

    int* container = malloc(k * sizeof(int));
    populate_array(container, k);

    int* answers = malloc(k * sizeof(int));
    MPI_Alltoall(container, 3, MPI_INT, answers, 3, MPI_INT, MPI_COMM_WORLD);

    for (int i = 0; i < k; ++i)
        printf("Process %d received %d-th element: %d\n", ProcRank, i, answers[i]);

    free(container);
    free(answers);
    MPI_Finalize();
    return 0;
}
