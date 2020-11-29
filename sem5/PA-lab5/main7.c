#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define true 1
#define false 0

#define array_upper_limit 10.0

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand(time(NULL) + ProcRank);

    int n;
    if ((ProcNum % 8) && (ProcNum % 12)) {
        printf("Wrong process number!\n");
        return 0;
    } else n = ProcNum / 4;

    double a = ((array_upper_limit / RAND_MAX) * rand());
    printf("Process %d: number is %f\n", ProcRank, a);

    MPI_Comm* low_comm, * high_comm, * mid_comm;
    low_comm = malloc(sizeof(MPI_Comm));
    high_comm = malloc(sizeof(MPI_Comm));
    mid_comm = malloc(sizeof(MPI_Comm));

    MPI_Cart_create(MPI_COMM_WORLD, 3, (int[3]) {2, 2, n},
                    (int[3]) {false, false, false}, false, low_comm);
    MPI_Cart_sub(*low_comm, (int[]) {false, true, true}, high_comm);
    MPI_Cart_sub(*high_comm, (int[]) {false, true}, mid_comm);

    int NProcNum, NProcRank;
    MPI_Comm_size(*mid_comm, &NProcNum);
    MPI_Comm_rank(*mid_comm, &NProcRank);

    double result;
    MPI_Reduce(&a, &result, 1, MPI_DOUBLE, MPI_PROD, 0, *mid_comm);
    if (NProcRank == 0) {
        printf("Root process (ex-%d) -> a numbers prod: %f\n", ProcRank, result);
    }

    free(mid_comm);
    free(high_comm);
    free(low_comm);
    MPI_Finalize();
    return 0;
}

