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
    int random;
    uint64_t array_length, threads_number;
    init_args(argc, argv, &random, &array_length, &threads_number);
    uint64_t* array = (uint64_t*) malloc(array_length * sizeof(uint64_t));

    uint64_t sorters_number = 3;
    const char* names[] = {"mergesort sequential", "mergesort optimized", "mergesort parallel"};
    void (*algorithms[4]) (uint64_t*, const uint64_t, const uint64_t) = {&sequential_merge_sort, &optimized_merge_sort, &parallel_merge_sort};

    printf("--> Sorting an array of size %lu with mergesort algorithm\n", array_length);
    if (random) printf("--> The array is initialized randomly\n");
    else printf("--> The array is initialized sequentially\n");

    for (uint64_t i = 0; i < sorters_number; i++) run_test(array, array_length, threads_number, random, names[i], algorithms[i]);
    test_algorithms(array, array_length, threads_number, random, sorters_number, names, algorithms);
    free(array);
}
