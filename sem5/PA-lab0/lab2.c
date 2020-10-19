#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define premier 1000000933
#define array_upper_limit 1000

void populate_array(unsigned int* arr, unsigned int size) {
    srand(time(NULL));
    printf("Array (length %d): [ ", size);
    for (unsigned int i = 0; i < size; i++) {
        arr[i] = (unsigned int) rand() % array_upper_limit;
        printf("%d ", arr[i]);
    }
    printf("]\n");
}

void not_array(unsigned int* arr, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) arr[i]--;
}

int main(int argc, char* argv[]) {
    int ProcNum, ProcRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int work_nodes = ProcNum - 1;

    if (ProcRank == 0) {
        double start_time = MPI_Wtime();
        printf ("Root process started at %lf\n", start_time);

        unsigned int array_size = (1u << (unsigned int) work_nodes) - 1u;
        printf("Calculated array size : %d\n", array_size);

        unsigned int* array = (unsigned int*) malloc(array_size * sizeof(unsigned int));
        populate_array(array, array_size);

        unsigned int* local_sizes = (unsigned int*) malloc(work_nodes * sizeof(unsigned int));
        unsigned int passing_elements = array_size / 2 + 1;
        unsigned int adv = premier % work_nodes;
        for (int i = 1; i < ProcNum; i++) {
            MPI_Send(&passing_elements, 1, MPI_UNSIGNED, i, ProcRank, MPI_COMM_WORLD);
            MPI_Send(array + passing_elements - 1, passing_elements, MPI_UNSIGNED, i, ProcRank, MPI_COMM_WORLD);
            local_sizes[i - 1] = passing_elements;
            passing_elements /= 2;
            adv = (adv + premier) % work_nodes;
        }

        printf("]\n");
        for (int i = 1; i < ProcNum; i++)
            MPI_Recv(&(array[local_sizes[i - 1] - 1]), local_sizes[i - 1], MPI_UNSIGNED, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &Status);

        free(local_sizes);

        printf("Array (length %d): [ ", array_size);
        for (unsigned int i = 0; i < array_size; i++) printf("%d ", array[i]);
        printf("]\n");

        free(array);

        double end_time = MPI_Wtime();
        printf("Root process ended at %lf\n", end_time);
        printf("All operations took %lf\n", end_time - start_time);

    } else {
        unsigned int local_size;
        MPI_Recv(&local_size, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &Status);
        unsigned int* arr = (unsigned int*) malloc(local_size * sizeof(unsigned int));
        MPI_Recv(arr, local_size, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &Status);
        not_array(arr, local_size);
        MPI_Send(arr, local_size, MPI_UNSIGNED, 0, ProcRank, MPI_COMM_WORLD);
        free(arr);
    }

    MPI_Finalize();
    return 0;
}
