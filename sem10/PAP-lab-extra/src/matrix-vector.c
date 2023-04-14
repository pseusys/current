#include <stdio.h>
#include <unistd.h>

#include "../libs/general.h"
#include "../libs/utils.h"


void matrixVector(int width, int height, double *A, double *B, double *C, int rank, int processes, MPI_Datatype* extra_type) {
    int tag = 0;
    int root = 0;
    int share = height / processes;
    int opsize = share * width;
    MPI_Scatter(A, opsize, MPI_DOUBLE, A, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);

    for (int p = 0; p < width; p++) {
        if (rank == 0)
            for (int i = 0; i < processes; i++)
                MPI_Send(&B[p], 1, *extra_type, i, tag, MPI_COMM_WORLD);
        else MPI_Recv(&B[p], 1, *extra_type, root, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (int i = 0; i < share; i++)
            for (int j = 0; j < width; j++)
                C[i * width + p] += A[i * width + j] * B[j * height + p];
    }

    MPI_Gather(C, opsize, MPI_DOUBLE, C, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
}

void on_start_impl(int mat_size, int processes, MPI_Datatype* extra_type) {
    MPI_Type_vector(mat_size, 1, mat_size, MPI_DOUBLE, extra_type);
	MPI_Type_commit(extra_type);
}

void on_end_impl(MPI_Datatype* extra_type) {
    MPI_Type_free(extra_type);
}

int main(int argc, char* argv[]) {
    eval_config config = {
        .eval = matrixVector,
        .start = on_start_impl,
        .end = on_end_impl
    };
    return eval(argc, argv, "virtual ring matrix-vector", &config);
}
