#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <x86intrin.h>

#include "sorting.h"


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

void parallel_quicksort(uint64_t *T, int64_t low, int64_t high) {
    #pragma omp parallel
    #pragma omp single
	if (low < high) {
		// pi = Partition index
		int64_t pi = partition(T, low, high);
        #pragma omp task
		parallel_quicksort(T, low, pi - 1);
        #pragma omp task
		parallel_quicksort(T, pi + 1, high);
        #pragma omp taskwait           
	}
}


int main (int argc, char **argv)
{
    struct timespec begin, end;
    double seconds;
    double nanosec;

    unsigned int exp ;

    /* the program takes one parameter N which is the size of the array to
       be sorted. The array will have size 2^N */
    if (argc != 2)
    {
        fprintf (stderr, "quicksort.run N \n") ;
        exit (-1) ;
    }

    uint64_t N = 1 << (atoi(argv[1])) ;
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
        
        serial_quicksort (X, 0, N - 1) ;

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

    printf ("\n quicksort serial \t\t\t %.3lf seconds\n\n", average_time()) ;    

  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif

        clock_gettime(CLOCK_MONOTONIC, &begin);
        
        parallel_quicksort (X, 0, N - 1) ;

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
    
    printf ("\n quicksort parallel \t\t\t %.3lf seconds\n\n", average_time()) ;


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

    serial_quicksort (Y, 0, N - 1) ;
    parallel_quicksort (Z, 0, N - 1) ;

    if (! are_vector_equals (Y, Z, N)) {
        fprintf(stderr, "ERROR: sorting with the sequential and the parallel algorithm does not give the same result\n") ;
        exit (-1) ;
    }


    free(X);
    free(Y);
    free(Z);
    
}
