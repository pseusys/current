#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define size 1000000

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    char* bytes = malloc(size * sizeof(char));

    if (ProcRank == 0) {
        printf("Processes exchange blocks, %d bytes each\n", size);
        double start_time = MPI_Wtime();
        printf("Root process started at %lf\n", start_time);

        MPI_Send(bytes, size, MPI_BYTE, 1, ProcRank, MPI_COMM_WORLD);
        MPI_Recv(bytes, size, MPI_BYTE, MPI_ANY_SOURCE, ProcNum - 1, MPI_COMM_WORLD, &Status);

        double end_time = MPI_Wtime();
        printf("Root process ended at %lf\n", end_time);
        printf("All transfer operations took %lf\n", end_time - start_time);

    } else {
        MPI_Recv(bytes, size, MPI_BYTE, MPI_ANY_SOURCE, ProcRank - 1, MPI_COMM_WORLD, &Status);
        printf("Process %d showed up at %lf\n", ProcRank, MPI_Wtime());
        int dest = ProcRank == ProcNum - 1 ? 0 : ProcRank + 1;
        MPI_Send(bytes, size, MPI_BYTE, dest, ProcRank, MPI_COMM_WORLD);
    }

    free(bytes);
    MPI_Finalize();
    return 0;
}
