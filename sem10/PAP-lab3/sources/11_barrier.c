#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size, tag = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int delay = rank + 1;
    printf("Process %d is going to sleep for %d seconds...\n", rank + 1, delay);
    sleep(delay);
    printf("Process %d has awoke!\n", rank + 1);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Process %d has passed the barrier!\n", rank + 1);

    MPI_Finalize();
    return 0;
}
