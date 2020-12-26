#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define array_upper_limit 1000000.0



int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand(time(NULL) + ProcRank);

    int nums [] = {(int) ((array_upper_limit / RAND_MAX) * rand()),
                   (int) ((array_upper_limit / RAND_MAX) * rand()),
                   (int) ((array_upper_limit / RAND_MAX) * rand())};
    printf("Process %d: a = %d, b = %d, c = %d\n", ProcRank, nums[0], nums[1], nums[2]);

    MPI_Comm* low_comm = malloc(sizeof(MPI_Comm));
    MPI_Comm_split(MPI_COMM_WORLD, ProcRank % 2, ProcRank, low_comm);

    if (!(ProcRank % 2)) {
        int NProcNum, NProcRank;
        MPI_Comm_size(*low_comm, &NProcNum);
        MPI_Comm_rank(*low_comm, &NProcRank);

        int *buffer = malloc(NProcNum * sizeof(int));
        MPI_Reduce(nums, buffer, 3, MPI_INT, MPI_MIN, 0, *low_comm);

        if (ProcRank == 0) for (char i = 'a'; i <= 'c'; ++i)
            printf("Root process: minimum for %c is %d\n", i, buffer[i - 'a']);

        free(buffer);
    }

    MPI_Finalize();
    return 0;
}
