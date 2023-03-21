#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

    int square_size = sqrt(size);
    MPI_Dims_create(square_size * square_size, flat, dimensions);
    MPI_Cart_create(MPI_COMM_WORLD, flat, dimensions, periods, reorder, &grid_communicator);

    if (grid_communicator != MPI_COMM_NULL) {
        int grid_rank;
        int* coords = (int*) malloc(2 * sizeof(int));
        MPI_Comm_rank(grid_communicator, &grid_rank);
        MPI_Cart_coords(grid_communicator, grid_rank, flat, coords);

        MPI_Comm row_communicator, col_communicator;
        int row_rank, col_rank;

        int row_split[] = {1, 0};
        MPI_Cart_sub(grid_communicator, row_split, &row_communicator);
        MPI_Comm_rank(row_communicator, &row_rank);

        int col_split[] = {0, 1};
        MPI_Cart_sub(grid_communicator, col_split, &col_communicator);
        MPI_Comm_rank(col_communicator, &col_rank);

        int broad;
        for (int i = 0; i < dimensions[0]; i++) {
            broad = col_rank;
            MPI_Bcast(&broad, 1, MPI_INT, row_rank, row_communicator);
            printf("Process [%d, %d] received broadcast from process with column rank %d\n", coords[0], coords[1], broad);
        }

        free(coords);
    }
 
    MPI_Finalize();
    return 0;
}
