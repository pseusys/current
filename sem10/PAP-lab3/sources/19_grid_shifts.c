#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

void print_process_neighbors(int* dimens, int* coords, int rank) {
    if (coords[0] == 0 && coords[0] == dimens[0] - 1) printf("Process %d is the only one in the row\n", rank);
    else if (coords[0] == 0) printf("Process %d is the leftmost one, has process [%d, %d] on the right\n", rank, coords[0] + 1, coords[1]);
    else if (coords[0] == dimens[0] - 1) printf("Process %d is the rightmost one, has process [%d, %d] on the left\n", rank, coords[0] - 1, coords[1]);
    else printf("Process %d has process [%d, %d] on the left and [%d, %d] on the right\n", rank, coords[0] + 1, coords[1], coords[0] - 1, coords[1]);
    if (coords[1] == 0 && coords[1] == dimens[1] - 1) printf("Process %d is the only one in the column\n", rank);
    else if (coords[1] == 0) printf("Process %d is the topmost one, has process [%d, %d] on the bottom\n", rank, coords[0], coords[1] + 1);
    else if (coords[1] == dimens[1] - 1) printf("Process %d is the botommost one, has process [%d, %d] on the top\n", rank, coords[0], coords[1] - 1);
    else printf("Process %d has process [%d, %d] on the bottom and [%d, %d] on the top\n", rank, coords[0], coords[1] + 1, coords[0], coords[1] - 1);
}

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

        int row_split[] = {0, 1};
        MPI_Cart_sub(grid_communicator, row_split, &row_communicator);
        MPI_Comm_rank(row_communicator, &row_rank);

        int col_split[] = {1, 0};
        MPI_Cart_sub(grid_communicator, col_split, &col_communicator);
        MPI_Comm_rank(col_communicator, &col_rank);

        printf("Process %d with coordinates [%d, %d] has rank %d on row and %d on column\n", grid_rank + 1, coords[0], coords[1], row_rank, col_rank);
        MPI_Barrier(grid_communicator);
        int source_rank, dest_rank;

        MPI_Cart_shift(row_communicator, 0, 1, &source_rank, &dest_rank);
        printf("After right shift process with rank %d with coordinates [%d, %d] has become rank %d\n", source_rank, coords[0], coords[1], dest_rank);
        print_process_neighbors(dimensions, coords, grid_rank);
        MPI_Barrier(grid_communicator);

        MPI_Cart_shift(col_communicator, 1, 1, &source_rank, &dest_rank);
        printf("After top shift process with rank %d with coordinates [%d, %d] has become rank %d\n", source_rank, coords[0], coords[1], dest_rank);
        print_process_neighbors(dimensions, coords, grid_rank);
        MPI_Barrier(grid_communicator);

        MPI_Cart_shift(row_communicator, 0, -1, &source_rank, &dest_rank);
        printf("After left shift process with rank %d with coordinates [%d, %d] has become rank %d\n", source_rank, coords[0], coords[1], dest_rank);
        print_process_neighbors(dimensions, coords, grid_rank);
        MPI_Barrier(grid_communicator);

        MPI_Cart_shift(col_communicator, 1, -1, &source_rank, &dest_rank);
        printf("After bottom shift process with rank %d with coordinates [%d, %d] has become rank %d\n", source_rank, coords[0], coords[1], dest_rank);
        print_process_neighbors(dimensions, coords, grid_rank);
        MPI_Barrier(grid_communicator);

        free(coords);
    }
 
    MPI_Finalize();
    return 0;
}
