#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <x86intrin.h>

#include "utils.h"

/* 
   merge sort -- sequential, parallel -- 
*/

void sequential_merge_sort (uint64_t *T, const uint64_t size, const uint64_t _) {
  if (size != 1) {
    uint64_t half_size = size / 2;
    sequential_merge_sort(T, half_size, _);
    sequential_merge_sort(T + half_size, half_size, _);
    merge(T, half_size);
  }
}

void parallel_merge_sort (uint64_t *T, const uint64_t size, uint64_t threads) {
  #pragma omp parallel num_threads(threads)
  #pragma omp single
  if (size != 1) {
    uint64_t half_size = size / 2;
    #pragma omp task
    parallel_merge_sort(T, half_size, threads);
    #pragma omp task
    parallel_merge_sort(T + half_size, half_size, threads);
    #pragma omp taskwait
    merge(T, half_size);
  }
}

void optimized_merge_sort (uint64_t *T, const uint64_t size, uint64_t threads) {
  uint64_t sorted = 1;
  #pragma omp parallel for num_threads(threads) schedule(static)
  for (uint64_t i = 1; i < size; i++) {
    if (T[i] < T[i-1]) sorted = 0;
  }
  if (sorted == 1) return;
  parallel_merge_sort(T, size, threads);
}


int main (int argc, char **argv) {
    int algorithm_number;
    uint64_t array_length, threads_number;
    init_args(argc, argv, &algorithm_number, &array_length, &threads_number);
    uint64_t* array = (uint64_t*) malloc(array_length * sizeof(uint64_t));

    uint64_t sorters_number = 3;
    const char* names[] = {"mergesort sequential", "mergesort optimized", "mergesort parallel"};
    void (*algorithms[3]) (uint64_t*, const uint64_t, const uint64_t) = {&sequential_merge_sort, &optimized_merge_sort, &parallel_merge_sort};

    if (VERB) {
        printf("--> Sorting an array of size %lu with mergesort algorithm\n", array_length);
        if (RINIT) printf("--> The array is initialized randomly\n");
        else printf("--> The array is initialized sequentially\n");
    } else printf("%s;%s;%s;\n", names[0], names[1], names[2]);

    if (algorithm_number == -1) {
        for (uint64_t i = 0; i < sorters_number; i++) run_test(array, array_length, threads_number, names[i], algorithms[i]);
        if (!VERB) printf("\n");
        test_algorithms(array, array_length, threads_number, sorters_number, names, algorithms);
    } else else run_test(array, array_length, threads_number, names[algorithm_number], algorithms[algorithm_number]);
    free(array);
}
