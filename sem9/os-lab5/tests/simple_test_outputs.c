#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../tasks.h"
#include "../debug.h"

#define NB_TASKS 8

#define VAL 38000

task_return_value_t simple_routine(task_t *t, unsigned int step)
{
    int *o = (int*) retrieve_output(t);
    *o = VAL;
    
    return TASK_COMPLETED;
}

int main(void)
{
    int i = 0;
    int *out_array[NB_TASKS];

    task_t *child;
    
    runtime_init();

    for(i=0; i<NB_TASKS; i++){
        child = create_task(simple_routine);
        out_array[i] = attach_output(child, sizeof(int));
        submit_task(child);
    }

    task_waitall();
    
    for(i=0; i<NB_TASKS; i++){
        if(*out_array[i] != VAL){
            PRINT_TEST_FAILED("Wrong output for %dth child task\n", i+1);
            exit(EXIT_FAILURE);
        }
    }
    
    PRINT_TEST_SUCCESS("Test executed successfully\n");
    
    runtime_finalize();

    return 0;
}
