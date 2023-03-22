#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <x86intrin.h>

#include "utils.h"

/* 
   bubble sort -- sequential, optimized, parallel -- 
*/

void sequential_bubble_sort (uint64_t *T, const uint64_t size, const uint64_t _) {
    uint64_t sorted;
    do {
        sorted = 0;
        for (size_t i = 0; i < size - 1; i++) {
            if (T[i] > T[i+1]) {
                uint64_t tmp = T[i];
                T[i] = T[i+1];
                T[i+1] = tmp;
                sorted = 1;
            }
        }
    } while (sorted == 1);
}

void parallel_bubble_sort (uint64_t *T, const uint64_t size, const uint64_t chunk) {
    int sorted;
    int chunk_size = size / chunk;

    do {
        sorted = 0;
        size_t i =0;
        #pragma omp parallel for num_threads(chunk) private(i) schedule(static)
        for (size_t i = 0; i < chunk; i++) {
            sequential_bubble_sort(T + chunk_size * i, chunk_size, chunk);
        }
        uint64_t lower;
        uint64_t upper;
        #pragma omp parallel for num_threads(chunk) private(i, lower, upper) schedule(static)
        for (size_t i = 1; i < chunk; i++) {
            uint64_t lower = chunk_size * i - 1;
            uint64_t upper = chunk_size * i;

            if (T[lower] > T[upper]) {
                uint64_t tmp = T[upper];
                T[upper] = T[lower];
                T[lower] = tmp;
                sorted = 1;
            }
        }
    } while (sorted == 1);
}

void optimized_bubble_sort (uint64_t *T, const uint64_t size, const uint64_t chunk) {
    int chunk_size = size / chunk;

    size_t i =0;
    #pragma omp parallel for num_threads(chunk) private(i) schedule(static)
    for (i = 0; i < chunk; i++) {
        sequential_bubble_sort(T + chunk_size * i, chunk_size, chunk);
    }
    for (i = chunk_size; i < size; i *= 2) {
        size_t increase = size / i / 2;
        size_t p = 0;
        #pragma omp parallel for num_threads(increase) private(p) schedule(static)
        for (p = 0; p < size; p += i * 2) {
            merge(T + p, i);
        }
    }
}


int main (int argc, char **argv) {
    int algorithm_number;
    uint64_t array_length, threads_number;
    init_args(argc, argv, &algorithm_number, &array_length, &threads_number);
    uint64_t* array = (uint64_t*) malloc(array_length * sizeof(uint64_t));

    uint64_t sorters_number = 3;
    const char* names[] = {"bubble sequential", "bubble optimized", "bubble parallel"};
    void (*algorithms[3]) (uint64_t*, const uint64_t, const uint64_t) = {&sequential_bubble_sort, &optimized_bubble_sort, &parallel_bubble_sort};

    if (VERB) {
        printf("--> Sorting an array of size %lu with bubblesort algorithm\n", array_length);
        if (RINIT) printf("--> The array is initialized randomly\n");
        else printf("--> The array is initialized sequentially\n");
    } else printf("%s;%s;%s;\n", names[0], names[1], names[2]);

    if (algorithm_number == -1) {
        for (uint64_t i = 0; i < sorters_number; i++) run_test(array, array_length, threads_number, names[i], algorithms[i]);
        if (!VERB) printf("\n");
        test_algorithms(array, array_length, threads_number, sorters_number, names, algorithms);
    } else run_test(array, array_length, threads_number, names[algorithm_number], algorithms[algorithm_number]);
    free(array);
}
