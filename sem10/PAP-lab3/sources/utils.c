#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void init_array_random_float(double *T, int size) {
    srand(time(NULL));
    for (int i = 0; i < size; i++) T[i] = (float) rand() / (float) (RAND_MAX / size);
}

void print_array_float(double *T, int size) {
    for (int i = 0; i < size; i++) printf("%.3lf ", T[i]);
    printf ("\n") ;
}

void init_array_random_int(int *T, int size) {
    srand(time(NULL));
    for (int i = 0; i < size; i++) T[i] = rand() % size;
}

void print_array_int(int *T, int size) {
    for (int i = 0; i < size; i++) printf("%d ", T[i]);
    printf ("\n") ;
}
