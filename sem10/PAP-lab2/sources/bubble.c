#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <x86intrin.h>

#include "sorting.h"

/* 
   bubble sort -- sequential, parallel -- 
*/

void sequential_bubble_sort (uint64_t *T, const uint64_t size) {
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
            sequential_bubble_sort(T + chunk_size * i, chunk_size);
        }
        uint64_t lower;
        uint64_t upper;
        #pragma omp parallel for num_threads(chunk) private(i, lower, upper) schedule(static)
        for (size_t i = 1; i <= chunk; i++) {
            uint64_t lower = chunk_size * i - 1;
            uint64_t upper = (lower == size - 1) ? 0 : chunk_size * i;

            if (T[lower] > T[upper]) {
                uint64_t tmp = T[upper];
                T[upper] = T[lower];
                T[lower] = tmp;
                sorted = 1;
            }
        }
    } while (sorted == 1);
}


int main (int argc, char **argv)
{
    struct timespec begin, end;
    double seconds;
    double nanosec;

    unsigned int exp ;

    /* the program takes one parameter N which is the size of the array to
       be sorted. The array will have size 2^N */
    if (argc != 3)
    {
        fprintf (stderr, "bubble.run N \n") ;
        exit (-1) ;
    }

    uint64_t arg = atoi(argv[1]);
    uint64_t N = *argv[1] ;
    uint64_t CH = *argv[2] ;
    /* the array to be sorted */
    uint64_t *X = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

    printf("--> Sorting an array of size %lu\n",N);
#ifdef RINIT
    printf("--> The array is initialized randomly\n");
#endif
    

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++){
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif
      
        clock_gettime(CLOCK_MONOTONIC, &begin);
        
        sequential_bubble_sort (X, N) ;

        clock_gettime(CLOCK_MONOTONIC, &end);
        
        seconds = end.tv_sec - begin.tv_sec;
        nanosec = end.tv_nsec - begin.tv_nsec;
        
        experiments [exp] = seconds + nanosec*1e-9;

        /* verifying that X is properly sorted */
#ifdef RINIT
        if (! is_sorted (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the sequential sorting of the array failed\n") ;
            exit (-1) ;
	}
#else
        if (! is_sorted_sequence (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the sequential sorting of the array failed\n") ;
            exit (-1) ;
	}
#endif
    }

    printf ("\n bubble serial \t\t\t %.3lf seconds\n\n", average_time()) ;    

  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif
        
        clock_gettime(CLOCK_MONOTONIC, &begin);

        parallel_bubble_sort (X, N, CH) ;

        clock_gettime(CLOCK_MONOTONIC, &end);
        
        seconds = end.tv_sec - begin.tv_sec;
        nanosec = end.tv_nsec - begin.tv_nsec;
        
        experiments [exp] = seconds + nanosec*1e-9;

        /* verifying that X is properly sorted */
#ifdef RINIT
        if (! is_sorted (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the parallel sorting of the array failed\n") ;
            exit (-1) ;
	}
#else
        if (! is_sorted_sequence (X, N))
        {
            print_array (X, N) ;
            fprintf(stderr, "ERROR: the parallel sorting of the array failed\n") ;
            exit (-1) ;
	}
#endif
                
        
    }
    
    printf ("\n bubble parallel \t\t %.3lf seconds\n\n", average_time()) ;    

    
    /* print_array (X, N) ; */

    /* before terminating, we run one extra test of the algorithm */
    uint64_t *Y = (uint64_t *) malloc (N * sizeof(uint64_t)) ;
    uint64_t *Z = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

#ifdef RINIT
    init_array_random (Y, N);
#else
    init_array_sequence (Y, N);
#endif

    memcpy(Z, Y, N * sizeof(uint64_t));

    sequential_bubble_sort (Y, N) ;
    parallel_bubble_sort (Z, N, CH) ;

    if (! are_vector_equals (Y, Z, N)) {
        fprintf(stderr, "ERROR: sorting with the sequential and the parallel algorithm does not give the same result\n") ;
        exit (-1) ;
    }


    free(X);
    free(Y);
    free(Z);
    
}
