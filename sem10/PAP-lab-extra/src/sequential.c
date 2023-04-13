#include "../libs/utils.h"


void sequential(int width, int height, double *A, double *B, double *C, int rank, int size) {
    if (rank == 0) matrixMultiplicationReference(width, height, A, B, C);
}

int main(int argc, char* argv[]) {
    return eval(argc, argv, "serial", sequential);
}
