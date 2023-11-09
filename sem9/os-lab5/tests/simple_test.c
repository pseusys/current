#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../tasks.h"
#include "../debug.h"

#define STR_SIZE 64
#define SLEEP_DURATION 2

/* Used to identify bugs */
int stage=0;

task_return_value_t print_message(task_t *t, unsigned int step)
{
    char *msg= (char*) retrieve_input(t);
    int *s = (int*) retrieve_input(t);
    
    printf("%s\n", msg);

    if(*s != stage){
        PRINT_TEST_FAILED("The semantic of some functions of the API is not correctly implemented\n");
        exit(EXIT_FAILURE);
    }

    return TASK_COMPLETED;
}

task_return_value_t sleep_task(task_t *t, unsigned int step)
{
    unsigned int *i = (unsigned int*) retrieve_input(t);
    
    sleep(*i);

    return TASK_COMPLETED;
}

int main(void)
{
    struct timespec begin, end;

    runtime_init();

    clock_gettime(CLOCK_MONOTONIC, &begin);
    
    task_t *t = create_task(print_message);

    char *in = attach_input(t, sizeof(char) * STR_SIZE);
    memset(in, 0, STR_SIZE);
    strncpy(in, "hello", STR_SIZE);

    int *s = attach_input(t, sizeof(int));
    *s = stage;

    submit_task(t);

    t = create_task(print_message);

    in = attach_input(t, sizeof(char) * STR_SIZE);
    memset(in, 0, STR_SIZE);
    strncpy(in, "hello again", STR_SIZE);

    s = attach_input(t, sizeof(int));
    *s = stage;
    
    submit_task(t);

    t = create_task(sleep_task);
    
    int *in2 = attach_input(t, sizeof(int));
    *in2 = SLEEP_DURATION;

    submit_task(t);

    task_waitall();

    stage++;

    t = create_task(print_message);

    in = attach_input(t, sizeof(char) * STR_SIZE);
    memset(in, 0, STR_SIZE);
    strncpy(in, "hello one last time", STR_SIZE);

    s = (int*) attach_input(t, sizeof(int));
    *s = stage;
    
    submit_task(t);

    clock_gettime(CLOCK_MONOTONIC, &end);
    
    runtime_finalize();

    /* basic test for correctness */
    double seconds = end.tv_sec - begin.tv_sec;
    
    if (seconds < SLEEP_DURATION){
        PRINT_TEST_FAILED("it seems that some tasks have not fully been executed\n");
    }
    else {
        PRINT_TEST_SUCCESS("simple_test executed successfully\n");
    }
    
    
    return 0;
}
