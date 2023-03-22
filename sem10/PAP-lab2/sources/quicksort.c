#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <x86intrin.h>

#include "utils.h"


/* 
   Merge two sorted chunks of array T!
   The two chunks are of size size
   First chunck starts at T[0], second chunck starts at T[size]
*/
int64_t partition(uint64_t *T, int64_t low, int64_t high) {
	uint64_t pivot = T[high];
	int64_t i = (low - 1);
    int64_t j;
	for (j = low; j <= high - 1; j++) {
		if (T[j] <= pivot) {
			i++;
            uint64_t temp = T[j];
            T[j] = T[i];
            T[i] = temp;
		}
	}
    uint64_t temp = T[i + 1];
    T[i + 1] = T[high];
    T[high] = temp;
	return (i + 1);
}


void serial_quicksort(uint64_t *T, int64_t low, int64_t high) {
	if (low < high) {
		// pi = Partition index
		int64_t pi = partition(T, low, high);
		serial_quicksort(T, low, pi - 1);
		serial_quicksort(T, pi + 1, high);           
	}
}

void serial_quicksort_wrapper(uint64_t *T, const uint64_t size, const uint64_t _) {
    serial_quicksort(T, 0, size - 1);
}

void parallel_quicksort(uint64_t *T, int64_t low, int64_t high, uint64_t threads) {
    #pragma omp parallel num_threads(threads)
    #pragma omp single
	if (low < high) {
		// pi = Partition index
		int64_t pi = partition(T, low, high);
        #pragma omp task
		parallel_quicksort(T, low, pi - 1, threads);
        #pragma omp task
		parallel_quicksort(T, pi + 1, high, threads);
        #pragma omp taskwait           
	}
}

void parallel_quicksort_wrapper(uint64_t *T, const uint64_t size, const uint64_t threads) {
    parallel_quicksort(T, 0, size - 1, threads);
}


int main (int argc, char **argv) {
    int algorithm_number;
    uint64_t array_length, threads_number;
    init_args(argc, argv, &algorithm_number, &array_length, &threads_number);
    uint64_t* array = (uint64_t*) malloc(array_length * sizeof(uint64_t));

    uint64_t sorters_number = 2;
    const char* names[] = {"quicksort sequential", "quicksort parallel"};
    void (*algorithms[4]) (uint64_t*, const uint64_t, const uint64_t) = {&serial_quicksort_wrapper, &parallel_quicksort_wrapper};

    printf("--> Sorting an array of size %lu with quicksort algorithm\n", array_length);
    if (RINIT) printf("--> The array is initialized randomly\n");
    else printf("--> The array is initialized sequentially\n");

    for (uint64_t i = 0; i < sorters_number; i++) run_test(array, array_length, threads_number, names[i], algorithms[i]);
    test_algorithms(array, array_length, threads_number, sorters_number, names, algorithms);
    free(array);
}
