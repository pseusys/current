#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define true 1
#define false 0

#define array_upper_limit 1000000.0

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand(time(NULL) + ProcRank);
    double start_time = MPI_Wtime();

    int n;
    if (ProcNum % 3) {
        if (ProcRank == 0) printf("Wrong process number!\n");
        return 0;
    } else n = ProcNum / 3;

    int a = (int) ((array_upper_limit / RAND_MAX) * rand());
    printf("Process %d: number is %d\n", ProcRank, a);

    MPI_Comm low_comm, high_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, (int[2]) {n, 3}, (int[2]) {false, false}, false, &low_comm);
    MPI_Cart_sub(low_comm, (int[]) {true, false}, &high_comm);

    int NProcNum, NProcRank;
    MPI_Comm_size(high_comm, &NProcNum);
    MPI_Comm_rank(high_comm, &NProcRank);

    int *buffer = malloc(NProcNum * sizeof(int));
    MPI_Gather(&a, 1, MPI_INT, buffer, 1, MPI_INT, 0, high_comm);
    if (NProcRank == 0) {
        printf("Root process (ex-%d) -> a numbers: {", ProcRank);
        for (int i = 0; i < NProcNum; ++i) {
            if (i != NProcNum - 1) printf(" %d,", buffer[i]);
            else printf(" %d", buffer[i]);
        }
        printf(" }\n");
    }
    if (ProcRank == 0) printf("It took %lf sec.\n", MPI_Wtime() - start_time);

    free(buffer);
    MPI_Finalize();
    return 0;
}
