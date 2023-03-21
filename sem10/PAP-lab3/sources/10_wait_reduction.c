#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size, tag = 0;
    int root = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root) {
        MPI_Request requests[size - 1];
        int results[size - 1];
        int sum = 0;
        for (int i = 1; i < size; i++) {
            MPI_Request* request = requests + i - 1;
            int* result = results + i - 1;
            MPI_Irecv(result, 1, MPI_INT, i, tag, MPI_COMM_WORLD, request);
        }
        MPI_Waitall(size - 1, requests, MPI_STATUSES_IGNORE);
        for (int i = 1; i < size; i++) sum += results[i - 1];
        printf("Process %d received sum of the other process ids: %d\n", rank + 1, sum);
    } else MPI_Send(&rank, 1, MPI_INT, root, tag, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
