#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../tasks.h"
#include "../parallel_for.h"
#include "../debug.h"

#define NB_TASKS 10
#define NB_INTERVALS 100000000

/* Code computing pi using a Riemann Sum to compute the integral
 * between O and 1 of '4 / (1 + x^2)'
 * See: https://dotink.co/posts/pi-by-riemann-sum/ */

int nb_intervals;
int nb_tasks;

void reduce_function(void *array, void *output, unsigned int size)
{
    double total = 0;
    double* arr = array;

    for(unsigned int i = 0; i<size; i++){
        total += arr[i];
    }

    *((double*) output) = total;
}

task_return_value_t for_routine(task_t *t, unsigned int step)
{
    for_inputs_t *in = (for_inputs_t*) retrieve_input(t);
    double *out = *(double**)retrieve_output(t);

    double count=0;
    double width = (double) 1 / nb_intervals;
    
    for(int i=in->begin_index; i< in->end_index; i += in->incr){
        double x = width * i;
        count += (double) 1/nb_intervals * (double) 4/(1 + x * x );
    }

    *out = count;

    return TASK_COMPLETED;
}



int main(int argc, char* argv[])
{
    struct timespec begin, end;
    double pi = 0;

    nb_intervals = NB_INTERVALS;
    nb_tasks = NB_TASKS;
    
    if(argc > 1){
        nb_intervals = atoi(argv[1]);
        if(argc > 2){
            nb_tasks = atoi(argv[2]);
        }
        if(argc > 3){
            fprintf(stderr,"Usage: %s (nb_intervals) (nb_tasks)\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    printf("Computing pi using %d intervals (%d tasks)\n", nb_intervals, nb_tasks);
    
    runtime_init();
    
    clock_gettime(CLOCK_MONOTONIC, &begin);
    parallel_for_with_reduction(for_routine, 0, nb_intervals, 1, nb_tasks, &pi, sizeof(double), reduce_function);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    printf("Result: %lf\n", pi);

    runtime_finalize();

    double resid = (pi > 3.14159)? pi - 3.14159: 3.14159 - pi;
    
    if(resid > 0.001){
        PRINT_TEST_FAILED("The computed value of pi seems to be wrong (or NB_INTERVALS is too small)\n");
    }
    else{
        PRINT_TEST_SUCCESS("The computed value of pi seems to be correct\n");
    }
    
    
    double seconds = end.tv_sec - begin.tv_sec;
    double nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;    
    printf("Time measured: %.3lf seconds.\n", elapsed);

    
    
    return 0;
}
