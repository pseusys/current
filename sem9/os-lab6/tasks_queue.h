#ifndef __TASKS_QUEUE_H__
#define __TASKS_QUEUE_H__

#include "tasks.h"


typedef struct tasks_queue{
    task_t** task_buffer;
    unsigned int task_buffer_size;
    unsigned int index;
} tasks_queue_t;


tasks_queue_t* create_tasks_queue(void);
void free_tasks_queue(tasks_queue_t *q);

void enqueue_task(tasks_queue_t *q, task_t *t);
task_t* dequeue_task(tasks_queue_t *q);

#endif
