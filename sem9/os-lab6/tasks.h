#ifndef __TASKS_H__
#define __TASKS_H__

#include <stddef.h>

#include "tasks_types.h"


/* Main functions of the API */

void runtime_init(void);
void runtime_init_with_deps(void);
void runtime_finalize(void);

task_t* create_task(task_routine_t f);
void submit_task(task_t *t);
void free_task(task_t* t);

void task_waitall(void);


/* Functions for managing the inputs and outputs of tasks */

void* attach_input(task_t *t, size_t s);
void* attach_output(task_t *t, size_t s);

void* retrieve_input(task_t *t);
void* retrieve_output(task_t *t);

void* retrieve_output_from_dependencies(task_t *t);

/* global variables that might be accessed from multiple files */
extern __thread task_t *active_task;
extern system_state_t sys_state;

#endif
