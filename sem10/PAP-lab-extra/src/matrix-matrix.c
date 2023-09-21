#include "../libs/general.h"


void matrixMatrix(int width, int height, double *A, double *B, double *C, int rank, int processes, MPI_Datatype* extra_type) {
    int tag = 0;
    int root = 0;
    int share = height / processes;
    int opsize = share * width;
    MPI_Scatter(A, opsize, MPI_DOUBLE, A, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Scatter(B, opsize, MPI_DOUBLE, B + opsize * rank, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);

    MPI_Request requests[2];
    // NOTICE: for convenience, "next" process is the one with the rank - 1, vice versa the "previous".
    int next = rank == 0 ? processes - 1 : rank - 1;
    int previous = rank == processes - 1 ? 0 : rank + 1;
    for (int p = 0; p < processes; p++) {
        int current = (p + rank) % processes;
        int recieving = (p + 1 + rank) % processes;
        MPI_Isend(&B[opsize * current], opsize, MPI_DOUBLE, next, tag, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&B[opsize * recieving], opsize, MPI_DOUBLE, previous, tag, MPI_COMM_WORLD, &requests[1]);

        for (int i = 0; i < share; i++)
            for (int j = 0; j < width; j++)
                for (int k = current * share; k < (current + 1) * share; k++)
                    C[i * width + j] += A[i * width + k] * B[k * height + j];
        
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    }

    MPI_Gather(C, opsize, MPI_DOUBLE, C, opsize, MPI_DOUBLE, root, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
    eval_config config = {.eval = matrixMatrix};
    return eval(argc, argv, "virtual ring matrix-matrix", &config);
}
