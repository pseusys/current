#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size, tag = 0;

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
        int* coords = (int*) malloc(2 * sizeof(int));

        int grid_rank;
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

        int token;
        if (coords[0] == 0 && coords[1] == 0) {
            token = 0xDEADBEEF;
            printf("Process [%d, %d] created a token: %d\n", coords[0], coords[1], token);
        } else {
            if (coords[0] == 0) {
                MPI_Recv(&token, 1, MPI_INT, coords[1] - 1, tag, col_communicator, MPI_STATUS_IGNORE);
                printf("Process [%d, %d] received a token from the process [%d, %d] in same column: %d\n", coords[0], coords[1], 0, coords[1] - 1, token);
            } else if (coords[1] == 0) {
                MPI_Recv(&token, 1, MPI_INT, coords[0] - 1, tag, row_communicator, MPI_STATUS_IGNORE);
                printf("Process [%d, %d] received a token from the process [%d, %d] in same row: %d\n", coords[0], coords[1], coords[0] - 1, 0, token);
            } else {
                MPI_Request requests[flat];
                int idx;
                MPI_Irecv(&token, 1, MPI_INT, coords[0] - 1, tag, row_communicator, requests);
                MPI_Irecv(&token, 1, MPI_INT, coords[1] - 1, tag, col_communicator, requests + 1);
                MPI_Waitany(flat, requests, &idx, MPI_STATUS_IGNORE);
                if (idx == 0) printf("Process [%d, %d] received a token from the process [%d, %d]: %d\n", coords[0], coords[1], coords[0] - 1, coords[1], token);
                else printf("Process [%d, %d] received a token from the process [%d, %d]: %d\n", coords[0], coords[1], coords[0], coords[1] - 1, token);
            }
        }

        if (coords[0] == dimensions[0] - 1 && coords[1] == dimensions[1] - 1) {
            printf("Process [%d, %d] is the last process and won't send the token anywhere\n", coords[0], coords[1]);
        } else if (coords[0] == dimensions[0] - 1) {
            MPI_Send(&token, 1, MPI_INT, coords[1] + 1, tag, col_communicator);
            printf("Process [%d, %d] sent the token to the process [%d, %d] in same column\n", coords[0], coords[1], coords[0], coords[1] + 1);
        } else if (coords[1] == dimensions[1] - 1) {
            MPI_Send(&token, 1, MPI_INT, coords[0] + 1, tag, row_communicator);
            printf("Process [%d, %d] sent the token to the process [%d, %d] in same row\n", coords[0], coords[1], coords[0] + 1, coords[1]);
        } else {
            MPI_Request requests[flat];
            MPI_Isend(&token, 1, MPI_INT, coords[0] + 1, tag, row_communicator, requests);
            MPI_Isend(&token, 1, MPI_INT, coords[1] + 1, tag, col_communicator, requests + 1);
            MPI_Waitall(flat, requests, MPI_STATUS_IGNORE);
            printf("Process [%d, %d] sent the token to the processes [%d, %d] and [%d, %d]\n", coords[0], coords[1], coords[0] + 1, coords[1], coords[0], coords[1] + 1);
        }

        free(coords);
    }
 
    MPI_Finalize();
    return 0;
}
