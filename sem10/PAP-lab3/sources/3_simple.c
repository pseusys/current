#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "utils.h"

int main(int argc, char** argv) {
    int rank, size, tag = 0;
    int array_size = 10;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double* array = (double*) malloc(array_size * sizeof(double));
    if (rank == 0) {
        int recepient = 1;
        init_array_random_float(array, array_size);
        printf("Process %d sent array: ", rank + 1);
        print_array_float(array, array_size);
        MPI_Send(array, array_size, MPI_DOUBLE, recepient, tag, MPI_COMM_WORLD);
    } else {
        int recepient = 0;
        MPI_Recv(array, array_size, MPI_DOUBLE, recepient, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received array: ", rank + 1);
        print_array_float(array, array_size);
    }
    free(array);
    
    MPI_Finalize();
    return 0;
}
