#ifndef SORTING_H
#define SORTING_H

#include <stdint.h>

#define NBEXPERIMENTS 10
extern double experiments[NBEXPERIMENTS];

/* utility functions */
void init_array_sequence(uint64_t *T, uint64_t size);
void init_array_random(uint64_t *T, uint64_t size);
void print_array(uint64_t *T, uint64_t size);
int is_sorted_sequence(uint64_t *T, uint64_t size);
int is_sorted(uint64_t *T, uint64_t size);
int are_vector_equals(uint64_t *T1, uint64_t *T2, uint64_t size);

/* merge function */
void merge(uint64_t *T, const uint64_t size);

/* returns the average time in seconds using the values stored in experiments vector */
double average_time();

void init_args(int argc, char **argv, int* random, uint64_t* array_length, uint64_t* threads_number);
void run_test(uint64_t* array, uint64_t array_length, uint64_t threads_number, int random, const char* name, void (*sorter) (uint64_t*, const uint64_t, const uint64_t));
void test_algorithms(uint64_t* array, uint64_t array_length, uint64_t threads_number, int random, uint64_t algorithms_number, const char** names, void (**sorters) (uint64_t*, const uint64_t, const uint64_t));

#endif // SORTING_H
