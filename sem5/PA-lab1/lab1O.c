#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void populate_array(unsigned int* arr, unsigned int size) {
    printf("Array (length %d): [ ", size);
    for (unsigned int i = 0; i < size; i++) {
        arr[i] = (unsigned int) rand();
        printf("%u, ", arr[i]);
    }
    printf("]\n");
}

unsigned int find_max(unsigned int* arr, unsigned int size) {
    if (size == 0) return -1;
    unsigned int max = arr[0];
    for (unsigned int i = 0; i < size; i++) if (arr[i] > max) max = arr[i];
    return max;
}

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int length = 100;

    if (ProcRank == 0) {
        printf ("Root process started\n");

        unsigned int* array = (unsigned int*) malloc(length * sizeof(unsigned int));
        populate_array(array, length);

        int steps = length / (ProcNum - 1);
        for (int i = 1; i < ProcNum; i++)
            MPI_Send(array + (i - 1) * steps, steps, MPI_UNSIGNED, i, ProcRank, MPI_COMM_WORLD);

        unsigned int* local_maxes = (unsigned int*) malloc(steps * sizeof(unsigned int));
        printf ("Local maxes received (one of each %d elements): [", steps);
        for (int i = 1; i < ProcNum; i++) {
            MPI_Recv(local_maxes + i * sizeof(unsigned int), 1, MPI_UNSIGNED, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &Status);
            printf("%u, ", *(local_maxes + i * sizeof(unsigned int)));
        }
        printf("]\n");

        free(array);
        unsigned int max = find_max(local_maxes, steps);
        free(local_maxes);
        printf("Global array max: %u\n", max);

    } else {
        unsigned int size = length / (ProcNum - 1);
        unsigned int* arr = (unsigned int*) malloc(size * sizeof(unsigned int));
        MPI_Recv(arr, size, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &Status);
        unsigned int local_max = find_max(arr, size);
        MPI_Send(&local_max, 1, MPI_UNSIGNED, 0, ProcRank, MPI_COMM_WORLD);
        free(arr);
    }

    MPI_Finalize();
    return 0;
}