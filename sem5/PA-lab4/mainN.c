#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>


int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    srand48(time(NULL) + ProcRank);

    double nums [] = {drand48() * 100, drand48() * 100, drand48() * 100};
    printf("Process %d: [%lf, %lf, %lf]\n", ProcRank, nums[0], nums[1], nums[2]);

    MPI_Comm* comm = malloc(sizeof(MPI_Comm));
    MPI_Comm_split(MPI_COMM_WORLD, ProcRank % 2 == 0 ? 1 : MPI_UNDEFINED, ProcRank, comm);

    if (!(ProcRank % 2)) {
        int NProcNum, NProcRank;
        MPI_Comm_size(*comm, &NProcNum);
        MPI_Comm_rank(*comm, &NProcRank);

        double *buffer = malloc(NProcNum * sizeof(double));
        MPI_Reduce(nums, buffer, 3, MPI_DOUBLE_PRECISION, MPI_MIN, 0, *comm);

        if (ProcRank == 0) {
            printf("\n");
            for (int i = 0; i < 3; ++i) printf("Root process: minimum for element %d is %lf\n", i + 1, buffer[i]);
        }

        free(buffer);
    }

    MPI_Finalize();
    return 0;
}

