#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "utils.h"

int comp (const void* elem1, const void* elem2) {
    return *((double*) elem1) > *((double*) elem2);
}

int main(int argc, char** argv) {
    int rank, size, tag = 0;
    int array_size = 10;
    double starttime, endtime;
    double MPI_Wtime();

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double* array = (double*) malloc(array_size * sizeof(double));
    if (rank == 0) {
        int recipient = 1;
        init_array_random_float(array, array_size);
        starttime = MPI_Wtime();
        MPI_Send(array, array_size, MPI_DOUBLE, recipient, tag, MPI_COMM_WORLD);
        printf("Process %d sent array: ", rank + 1);
        print_array_float(array, array_size);
        MPI_Recv(array, array_size, MPI_DOUBLE, recipient, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received array: ", rank + 1);
        endtime   = MPI_Wtime();
        print_array_float(array, array_size);
        printf("That took %f seconds\n",endtime-starttime);
    } else {
        int recipient = 0;
        MPI_Recv(array, array_size, MPI_DOUBLE, recipient, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received array: ", rank + 1);
        print_array_float(array, array_size);
        qsort(array, array_size, sizeof(double), comp);
        printf("Process %d sorted array: ", rank + 1);
        print_array_float(array, array_size);
        MPI_Send(array, array_size, MPI_DOUBLE, recipient, tag, MPI_COMM_WORLD);
    }
    free(array);
    
    MPI_Finalize();
    return 0;
}
