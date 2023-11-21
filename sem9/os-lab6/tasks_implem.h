#ifndef __TASKS_IMPLEM_H__
#define __TASKS_IMPLEM_H__

#include <stdbool.h>

#include "tasks_types.h"

extern bool ready_to_terminate;
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t queue_finished;

void create_queues(void);
void delete_queues(void);

void create_thread_pool(void);

void dispatch_task(task_t *t);
task_t* get_task_to_execute(void);
unsigned int exec_task(task_t *t);
void terminate_task(task_t *t);

void task_check_runnable(task_t *t);

#endif
