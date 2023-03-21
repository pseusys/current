#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int root = 0;
    int result;

    printf("Process %d has id: %d\n", rank + 1, rank);
    MPI_Reduce(&rank, &result, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
    if (rank == root) printf("Process %d received sum of the other process ids: %d\n", rank + 1, result);
    
    MPI_Finalize();
    return 0;
}
