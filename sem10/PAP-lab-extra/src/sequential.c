#include "../libs/general.h"


void sequential(int width, int height, double *A, double *B, double *C, int rank, int processes, MPI_Datatype* extra_type) {
    if (rank == 0) matrix_multiplication_default(width, height, A, B, C);
}

int main(int argc, char* argv[]) {
    eval_config config = {.eval = sequential};
    return eval(argc, argv, "serial", &config);
}
