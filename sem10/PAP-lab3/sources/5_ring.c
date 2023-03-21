#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size, tag = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        long int token = 1;
        int sender = size - 1;
        int recipient = rank + 1;
        printf("Process %d requests factorial of the ring size\n", rank + 1);
        MPI_Send(&token, 1, MPI_LONG, recipient, tag, MPI_COMM_WORLD);
        MPI_Recv(&token, 1, MPI_LONG, sender, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received factorial: %ld\n", rank + 1, token);
    } else {
        long int token;
        int sender = rank - 1;
        int recipient = rank == size - 1 ? 0 : rank + 1;
        MPI_Recv(&token, 1, MPI_LONG, sender, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received value: %ld\n", rank + 1, token);
        token *= rank + 1;
        printf("Process %d modified value: %ld\n", rank + 1, token);
        MPI_Send(&token, 1, MPI_LONG, recipient, tag, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
