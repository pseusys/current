#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int ranger = ProcRank * 10;
    int quadron [] = {ranger + 1, ranger + 2, ranger + 3, ranger + 4};

    int* buffer = malloc(ProcNum * sizeof(quadron));
    MPI_Allgather(&quadron, 4, MPI_INT, buffer, 4, MPI_INT, MPI_COMM_WORLD);
    for (int i = 0; i < ProcNum * 4; ++i) printf("Process %d said: %d\n", ProcRank, buffer[i]);

    free(buffer);
    MPI_Finalize();
    return 0;
}
