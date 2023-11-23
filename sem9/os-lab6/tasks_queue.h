#ifndef __TASKS_QUEUE_H__
#define __TASKS_QUEUE_H__

#include <pthread.h>

#include "tasks.h"


typedef struct tasks_queue_block{
    struct tasks_queue_block *previous, *next;
    task_t* task;
} tasks_queue_block_t;

typedef struct tasks_queue{
    tasks_queue_block_t *head, *tail;
    unsigned int length;
    pthread_mutex_t queue_mutext;
    pthread_cond_t queue_not_empty;
} tasks_queue_t;


tasks_queue_t* create_tasks_queue(void);
void free_tasks_queue(tasks_queue_t *q);

void push_task(tasks_queue_t *q, task_t *t);
task_t* pop_task(tasks_queue_t *q);
task_t* remove_task(tasks_queue_t *q);

#endif
