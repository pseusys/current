#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include "utils.h"

int comp (const void* elem1, const void* elem2) {
    return *((double*) elem1) > *((double*) elem2);
}

int main(int argc, char** argv) {
    int rank, size, tag = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char* message = strdup("MessageFromProcessN");
    int length = strlen(message);
    message[length - 1] = rank + 1 + '0';

    MPI_Request request;
    int recipient = rank == 0 ? 1 : 0;

    MPI_Isend(message, length, MPI_CHAR, recipient, tag, MPI_COMM_WORLD, &request);
    printf("Process %d started sending message asynchronously: %s\n", rank + 1, message);
    printf("Process %d started receiving message asynchronously.\n", rank + 1);
    MPI_Irecv(message, length, MPI_CHAR, recipient, tag, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);
    printf("Process %d received message asynchronously: %s.\n", rank + 1, message);
    
    free(message);
    MPI_Finalize();
    return 0;
}
