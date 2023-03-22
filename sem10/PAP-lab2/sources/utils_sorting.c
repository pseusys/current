#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

#ifndef CLOCK_MONOTONIC
    #define CLOCK_MONOTONIC 1
#endif

void init_args(int argc, char **argv, int* algo_number, uint64_t* array_length, uint64_t* threads_number) {
    /* the program takes one parameter N which is the size of the array to be sorted, the array will have size 2^N */
    if (!(argc == 4 || argc == 3)) {
        fprintf(stderr, "[algorithm].run [array_length] [threads_number] OR [algorithm].run [array_length] [threads_number] [algorithm_number or -1 for all algorithms]\n");
        exit(-1);
    }
    *array_length = 1 << atoi(argv[1]);
    *threads_number = atoi(argv[2]);
    *algo_number = argc == 4 ? atoi(argv[3]) : -1;
}

void test_sorted(uint64_t* array, uint64_t array_length, const char* round) {
    /* verifying that X is properly sorted */
    if ((RINIT && !is_sorted(array, array_length)) || (!RINIT && !is_sorted_sequence (array, array_length))) {
        print_array(array, array_length);
        fprintf(stderr, "ERROR: the %s sorting of the array failed\n", round);
        exit(-1);
	}
}

void run_test(uint64_t* array, uint64_t array_length, uint64_t threads_number, const char* name, void (*sorter) (uint64_t*, const uint64_t, const uint64_t)) {
    int quick_break = 0;
    struct timespec begin, end;
    double experiments[NBEXPERIMENTS];
    for (uint64_t exp = 0; exp < NBEXPERIMENTS; exp++) {
        if (RINIT) init_array_random(array, array_length);
        else init_array_sequence(array, array_length);
      
        clock_gettime(CLOCK_MONOTONIC, &begin);
        sorter(array, array_length, threads_number);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        double seconds = end.tv_sec - begin.tv_sec;
        double nanosec = end.tv_nsec - begin.tv_nsec;
        experiments[exp] = seconds + nanosec*1e-9;

        if (seconds > 1.0) {
            quick_break = 1;
            break;
        }
        test_sorted(array, array_length, "sequential");
    }
    if (VERB) {
        if (quick_break) printf("\n %s \t\t\t more than a second\n\n", name);
        else printf("\n %s \t\t\t %.3lf seconds\n\n", name, average_time(experiments, NBEXPERIMENTS));
    } else {
        if (quick_break) printf("None;");
        else printf("%.3lf;", average_time(experiments, NBEXPERIMENTS));
    }
}

void test_algorithms(uint64_t* array, uint64_t array_length, uint64_t threads_number, uint64_t algorithms_number, const char** names, void (**sorters) (uint64_t*, const uint64_t, const uint64_t)) {
    if (RINIT) init_array_random(array, array_length);
    else init_array_sequence(array, array_length);

    uint64_t* sorted = (uint64_t*) malloc(array_length * sizeof(uint64_t));
    memcpy(sorted, array, array_length * sizeof(uint64_t));
    sorters[0](sorted, array_length, threads_number);
    test_sorted(sorted, array_length, names[0]);
    if (algorithms_number == 1) return;

    for (uint64_t i = 1; i < algorithms_number; i++) {
        uint64_t* copy = (uint64_t*) malloc(array_length * sizeof(uint64_t));
        memcpy(copy, array, array_length * sizeof(uint64_t));
        sorters[i](copy, array_length, threads_number);
        if (!are_vector_equals(sorted, copy, array_length)) {
            fprintf(stderr, "ERROR: sorting with the %s and %s algorithms does not give the same result\n", names[0], names[i]);
            exit(-1);
        }
        free(copy);
    }
    free(sorted);
}
