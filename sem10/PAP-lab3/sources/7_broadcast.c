#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "utils.h"

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int array_size = 10;
    double* array = (double*) malloc(array_size * sizeof(double));
    
    int root = 0;
    if (rank == root) {
        init_array_random_int(array, array_size);
        printf("Process %d sent array: ", rank + 1);
        print_array_int(array, array_size);
    }

    MPI_Bcast(array, array_size, MPI_INT, root, MPI_COMM_WORLD);
    printf("Process %d received array: ", rank + 1);
    print_array_int(array, array_size);
    free(array);
    
    MPI_Finalize();
    return 0;
}
