#ifndef __TASKS_IMPLEM_H__
#define __TASKS_IMPLEM_H__

#include <stdbool.h>

#include "tasks_types.h"

void create_queues(void);
void delete_queues(void);

void create_thread_pool(void);

void dispatch_task(task_t *t);
unsigned int exec_task(task_t *t);
void terminate_task(task_t *t);

void task_check_runnable(task_t *t);

#endif
