#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "mpi.h"

double pi_value(double value) {
    return 4.0 / (1.0 + value * value);
}

double pi_integral(double lower, double higher) {
    return (pi_value(lower) + pi_value(higher)) / 2.0;
}

int main(int argc, char** argv) {
    int rank, size, tag = 0;
    int root = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int intervals;
    // There might be "scanf("%d", &myInt);", but it was replaced with console arg for easier testing.
    if (rank == 0) intervals = atoi(argv[1]);

    MPI_Bcast(&intervals, 1, MPI_INT, root, MPI_COMM_WORLD);
    double* array = (double*) malloc((intervals + 1) * sizeof(double));

    if (rank == 0) {
        double step = 1.0 / intervals;
        for (int i = 0; i <= intervals; i++) array[i] = step * i;
        array[intervals] = 1.0;
    }

    MPI_Bcast(array, intervals + 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    int share = ceil((double) intervals / (double) size);
    double answer = 0.0;

    for (int i = 0; i < share; i++) {
        int current_interval = share * rank + i;
        if (current_interval >= intervals) break;
        answer += pi_integral(array[current_interval], array[current_interval + 1]) / (double) intervals;
    }

    double result;
    MPI_Reduce(&answer, &result, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
    if (rank == 0) printf("Computed value of PI (on %d intervals): %.7lf\n", intervals, result);
    free(array);

    MPI_Finalize();
    return 0;
}

