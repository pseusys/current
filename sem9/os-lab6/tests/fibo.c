#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

#include "../tasks.h"
#include "../debug.h"

int fibo_sequential(int number)
{
    if(number == 0){
        return 0;
    }

    if(number == 1){
        return 1;
    }

    int result = 0;
    int *fib = (int*) malloc((number) * sizeof(int));

    fib[0] = 0;
    fib[1] = 1;

    for(int i=2; i< number; i++){
        fib[i] = fib[i-1] + fib[i-2];
    }
    result = fib[number-1] + fib[number-2];

    free(fib);

    return result;
}




/* Recursive computation of Fibinacci numbers */

task_return_value_t fibo_task(task_t *t, unsigned int step)
{

    switch(step){

    case 1:;
        
        int n = *((int*)retrieve_input(t));
    
        if (n < 2) {
            int *o = (int*) retrieve_output(t);
            *o = n;
            
            return TASK_COMPLETED;
            
        } else {
            task_t *child1 = create_task(fibo_task);
            int *in = attach_input(child1, sizeof(int));
            *in = n-1;
            attach_output(child1, sizeof(int));
            submit_task(child1);

            task_t *child2 = create_task(fibo_task);
            in = attach_input(child2, sizeof(int));
            *in = n-2;
            attach_output(child2, sizeof(int));
            submit_task(child2);

        }

        break;

    case 2:;
        
        int* r1 = retrieve_output_from_dependencies(t);
        int* r2 = retrieve_output_from_dependencies(t);
        int *o = (int*) retrieve_output(t);
        
        *o = *r1 + *r2;

        return TASK_COMPLETED;
        
    default:
        PRINT_TEST_FAILED("Error: No step %d for this task\n", step);
        assert(0);
    }
    

    return TASK_TO_BE_RESUMED;
}

void verify_fibo(int index, int res)
{
    int expected_result = fibo_sequential(index);
    if(res == expected_result){
        PRINT_TEST_SUCCESS("Correct result for Fibo %d: %d\n", index, res);
    }
    else{
        PRINT_TEST_FAILED("Wrong result for Fibo %d: %d instead of %d\n", index, res, expected_result);
    }
}


int main(int argc, char* argv[])
{
    struct timespec begin, end;

    if(argc != 2){
        fprintf(stderr,"Usage: %s index\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int index = atoi(argv[1]);
    
    runtime_init_with_deps();

    clock_gettime(CLOCK_MONOTONIC, &begin);
    
    task_t *t = create_task(fibo_task);

    int *in = attach_input(t, sizeof(int));
    *in = index;

    int* out = attach_output(t, sizeof(int));
    
    submit_task(t);

    task_waitall();

    clock_gettime(CLOCK_MONOTONIC, &end);
    
    verify_fibo(index, *out);
    
    runtime_finalize();

    double seconds = end.tv_sec - begin.tv_sec;
    double nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;    
    printf("Time measured: %.3lf seconds.\n", elapsed);
    
    return 0;
}
