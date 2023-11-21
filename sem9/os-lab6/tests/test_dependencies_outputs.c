#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "../tasks.h"
#include "../debug.h"

#define NB_CHILDREN 8

#define VAL 38000

task_return_value_t child_routine(task_t *t, unsigned int step)
{
    int *o = (int*) retrieve_output(t);
    *o = VAL;
    
    return TASK_COMPLETED;
}

task_return_value_t parent_routine(task_t *t, unsigned int step)
{
    int i=0;
    
    switch(step){

    case 1:;
        task_t *child = NULL;

        for(i=0; i<NB_CHILDREN; i++){
            child = create_task(child_routine);
            attach_output(child, sizeof(int));
            submit_task(child);
        }
        
        break;

    case 2:;
        int* r;
        
        for(i=0; i<NB_CHILDREN; i++){
           r = retrieve_output_from_dependencies(t);
           if(*r != VAL){
               PRINT_TEST_FAILED("Wrong output for %dth child task\n", i+1);
               exit(EXIT_FAILURE);
           }
        }
        
        return TASK_COMPLETED;

    default:
        PRINT_TEST_FAILED("Error: No step %d for this task\n", step);
        assert(0);
    }
    
    return TASK_TO_BE_RESUMED;
}

int main(void)
{
    runtime_init_with_deps();

    task_t *t = create_task(parent_routine);
    submit_task(t);
    
    runtime_finalize();

    PRINT_TEST_SUCCESS("Test executed successfully\n");
    
    return 0;
}
