#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define array_size 1000000
#define array_upper_limit 1000.0

void populate_array(double* arr, unsigned int size) {
    srand(time(NULL));
    //printf("Array (length %d): [ ", size);
    for (unsigned int i = 0; i < size; i++) {
        arr[i] = (array_upper_limit / RAND_MAX) * rand();
        //printf("%lf ", arr[i]);
    }
    //printf("]\n");
}

double sum_array(const double* arr, unsigned int size) {
    if (size == 0) return -1;
    double max = arr[0];
    for (unsigned int i = 0; i < size; i++) if (arr[i] > max) max = arr[i];
    return max;
}

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int work_nodes = ProcNum - 1;
    int node_quote = array_size / work_nodes;

    if (ProcRank == 0) {
        double start_time = MPI_Wtime();
        //printf ("Root process started at %lf\n", start_time);

        double* array = (double*) malloc(array_size * sizeof(double));
        populate_array(array, array_size);

        for (int i = 1; i < ProcNum; i++)
            MPI_Send(array + (i - 1) * node_quote, node_quote, MPI_DOUBLE, i, ProcRank, MPI_COMM_WORLD);

        double* local_sums = (double*) malloc(work_nodes * sizeof(double));
        //printf ("Local maxes received (one of each %d elements): [ ", node_quote);
        for (int i = 1; i < ProcNum; i++) {
            MPI_Recv(&(local_sums[i - 1]), 1, MPI_DOUBLE, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &Status);
            //printf("%lf ", local_sums[i - 1]);
        }
        //printf("]\n");

        free(array);
        double max = sum_array(local_sums, ProcNum);
        free(local_sums);
        //printf("Global array max: %lf\n", max);

        double end_time = MPI_Wtime();
        //printf("Root process ended at %lf\n", end_time);
        printf("For %d processes and %d elements it took %lf sec.\n", ProcNum, array_size, end_time - start_time);

    } else {
        double* arr = (double*) malloc(node_quote * sizeof(double));
        MPI_Recv(arr, node_quote, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &Status);
        double local_sum = sum_array(arr, node_quote);
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, ProcRank, MPI_COMM_WORLD);
        free(arr);
    }

    MPI_Finalize();
    return 0;
}
