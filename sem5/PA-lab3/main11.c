#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define array_upper_limit 1000000.0

typedef struct {
    int value;
    int index;
} integex;

void populate_integex_array(integex* arr, int size, int rank) {
    srand(time(NULL) + rank);
    for (int i = 0; i < size; i++) {
        arr[i].index = rank;
        arr[i].value = (int) ((array_upper_limit / RAND_MAX) * rand());
    }
}

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int k = ProcNum + 5;

    integex* containment = malloc(k * sizeof(integex));
    populate_integex_array(containment, k, ProcRank);

    printf("Process %d: {", ProcRank);
    for (int i = 0; i < k; ++i) {
        if (i != k - 1) printf(" %d,", containment[i].value);
        else printf(" %d", containment[i].value);
    }
    printf(" }\n");

    integex* answers = malloc(k * sizeof(integex));
    MPI_Allreduce(containment, answers, k, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);

    if (ProcRank == 0) for (int i = 0; i < k; ++i)
        printf("Root process: minimum for index %d is %d\n", i, answers[i].value);
    else for (int i = 0; i < k; ++i)
        printf("Process %d: minimum for index %d is in process %d\n", ProcRank, i, answers[i].index);

    free(containment);
    free(answers);
    MPI_Finalize();
    return 0;
}
