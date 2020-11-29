#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define true 1
#define false 0

/**
 * For 8 processes:
 * |---|---|
 * | 0 | 4 |
 * |---|---|
 * | 1 | 5 |
 * |---|---|
 * | 2 | 6 |
 * |---|---|
 * | 3 | 7 |
 * |---|---|
 */

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand(time(NULL) + ProcRank);

    int n;
    if (ProcNum % 2) {
        printf("Odd process number!\n");
        return 0;
    } else n = ProcNum / 2;

    int a = ProcRank + 116;

    MPI_Comm* low_comm = malloc(sizeof(MPI_Comm));
    MPI_Cart_create(MPI_COMM_WORLD, 2, (int[2]) {2, n}, (int[2]) {true, false}, false, low_comm);

    int giver, getter;
    MPI_Cart_shift(*low_comm, 0, 1, &giver, &getter);

    int new_a;
    MPI_Sendrecv(&a, 1, MPI_INT, getter, ProcRank, &new_a, 1, MPI_INT, giver, MPI_ANY_TAG, *low_comm, &Status);

    printf("Process %d said: %d\n", ProcRank, new_a);

    free(low_comm);
    MPI_Finalize();
    return 0;
}
