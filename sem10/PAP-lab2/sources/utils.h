#ifndef SORTING_H
#define SORTING_H

#include <stdint.h>

#define NBEXPERIMENTS 10

#ifndef RINIT
    #define RINIT 0
#else
    #define RINIT 1
#endif

#ifndef VERB
    #define VERB 0
#else
    #define VERB 1
#endif

typedef void (*sorter_function) (uint64_t*, const uint64_t, const uint64_t);

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
double average_time(double* experiments, uint64_t experiments_length);

void init_args(int argc, char **argv, int* algo_number, uint64_t* array_length, uint64_t* threads_number);
void run_test(uint64_t* array, uint64_t array_length, uint64_t threads_number, const char* name, sorter_function sorter);
void test_algorithms(uint64_t* array, uint64_t array_length, uint64_t threads_number, uint64_t algorithms_number, const char** names, sorter_function* sorter);

#endif // SORTING_H
