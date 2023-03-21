#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm grid_communicator;
    int flat = 2;
    int dimensions[] = {0, 0};
    int periods[] = {0, 0};
    int reorder = 0;

    MPI_Dims_create(size, flat, dimensions);
    MPI_Cart_create(MPI_COMM_WORLD, flat, dimensions, periods, reorder, &grid_communicator);

    int grid_rank;
    int* coords = (int*) malloc(2 * sizeof(int));
    MPI_Comm_rank(grid_communicator, &grid_rank);
    MPI_Cart_coords(grid_communicator, grid_rank, flat, coords);
    printf("Process %d (%d) has coordinates [%d, %d]\n", grid_rank + 1, rank + 1, coords[0], coords[1]);
 
    free(coords);
    MPI_Finalize();
    return 0;
}
