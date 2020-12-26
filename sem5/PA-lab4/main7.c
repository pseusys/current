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

    double start_time = MPI_Wtime();

    int a = rand() % 10000;
    int n = ProcRank < 2 ? ProcRank : (rand() % 2);
    printf("Process %d (n = %d): a = %d\n", ProcRank, n, a);

    MPI_Comm low_comm;
    MPI_Comm_split(MPI_COMM_WORLD, n, ProcRank, &low_comm);

    if (n == 1) {
        int NProcNum, NProcRank;
        MPI_Comm_size(low_comm, &NProcNum);
        MPI_Comm_rank(low_comm, &NProcRank);

        int *buffer = malloc(NProcNum * sizeof(int));
        MPI_Gather(&a, 1, MPI_INT, buffer, 1, MPI_INT, 0, low_comm);
        if (NProcRank == 0) {
            printf("Root process -> a numbers: {");
            for (int i = 0; i < NProcNum; ++i) {
                if (i != NProcNum - 1) printf(" %d,", buffer[i]);
                else printf(" %d", buffer[i]);
            }
            printf(" }\nIt took %lf sec.\n", MPI_Wtime() - start_time);
        }

        free(buffer);
    }
    MPI_Finalize();
    return 0;
}
