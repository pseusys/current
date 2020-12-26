#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand(time(NULL) + ProcRank);

    int a = ProcRank + 116;
    int n = ProcRank + 1 < 3 ? ProcRank + 1 : (rand() % 2) + 1;

    MPI_Comm* low_comm = malloc(sizeof(MPI_Comm));
    MPI_Comm_split(MPI_COMM_WORLD, n, ProcRank, low_comm);

    int NProcNum, NProcRank;
    MPI_Comm_size(*low_comm, &NProcNum);
    MPI_Comm_rank(*low_comm, &NProcRank);

    int* buffer = malloc(NProcNum * sizeof(int));
    MPI_Allgather(&a, 1, MPI_INT, buffer, 1, MPI_INT, *low_comm);
    for (int i = 0; i < NProcNum; ++i)
        printf("Process %d -> %d from group of %d -> %d elements said: %d\n",
               ProcRank, NProcRank, ProcNum, NProcNum, buffer[i]);

    free(buffer);
    free(low_comm);
    MPI_Finalize();
    return 0;
}
