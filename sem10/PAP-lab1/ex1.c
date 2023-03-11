#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void hello(char *st) {
    int my_rank, nthreads, nmaxthreads;

    // to get the max number of threads
    nmaxthreads = omp_get_max_threads();

    // to get the current number of threads
    nthreads = omp_get_num_threads();

    // to get my rank
    my_rank = omp_get_thread_num();
	
    printf("%s thread %d of team %d (max_num_threads is %d)\n", st, my_rank, nthreads,nmaxthreads);
} 


int main (int argc, char*argv[]) {
    // getting the thread number (program parameter)
    if (argc != 2) {
        fprintf(stderr, "the parameter (thread number) is missing!\n");
        fprintf(stderr, "exo1 thread_number\n");
        exit(-1);
    }

    // program starts here with the primary thread
    int nthreads = atoi(argv[1]);
  
    printf("I am the primary thread %d and I start\n", omp_get_thread_num ());

    printf("Starting Region 1 \n");
    // this is a parallel region, all threads will execute this region
    #pragma omp parallel num_threads (nthreads)
    hello("Region 1 ");
    printf("End of Region 1\n");

    printf("Starting Region 2 \n");
    // this is a parallel region, half of threads will execute this region
    #pragma omp parallel num_threads (nthreads/2) 
    hello("Region 2 ") ;
    // end of the parallel region, the primary thread is alone in this serial region
    printf("End of Region 2\n");

    printf("Starting Region 3 \n");
    // this is a parallel region, quarter of threads will execute this region
    #pragma omp parallel num_threads (nthreads/4) 
    hello("Region 3 ");
    // end of the parallel region, the primary thread is alone in this serial region
    printf("End of Region 3\n");
  
    printf("I am the primary thread %d and I complete\n", omp_get_thread_num());
    return 0;
} 


