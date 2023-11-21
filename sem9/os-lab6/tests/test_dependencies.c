#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "../tasks.h"
#include "../debug.h"

/* Used to identify bugs */
int stage = 0;

task_return_value_t child_routine(task_t *t, unsigned int step)
{
    printf("Hi from child\n");

    stage++;
    
    return TASK_COMPLETED;
}

task_return_value_t parent_routine(task_t *t, unsigned int step)
{
    switch(step){

    case 1:;
        printf("Hello from parent (before child)\n");

        task_t *child = create_task(child_routine);
        submit_task(child);

        break;

    case 2:;
        printf("Hello from parent (after child)\n");

        if(stage != 1){
            PRINT_TEST_FAILED("The semantic of some functions of the API is not correctly implemented\n");
            exit(EXIT_FAILURE);
        }

        stage++;
        
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

    if(stage != 2){
        PRINT_TEST_FAILED("Error: The semantic of some functions of the API is not correctly implemented\n");
        exit(EXIT_FAILURE);
    }

    PRINT_TEST_SUCCESS("Test executed successfully\n");

    return 0;
}
