#include <stdio.h>
#include <stdlib.h>

#include "tasks_queue.h"


// TODO: make a list


tasks_queue_t* create_tasks_queue(void)
{
    tasks_queue_t *q = (tasks_queue_t*) malloc(sizeof(tasks_queue_t));

    q->task_buffer_size = QUEUE_SIZE;
    q->task_buffer = (task_t**) malloc(sizeof(task_t*) * q->task_buffer_size);

    q->index = 0;

    return q;
}


void free_tasks_queue(tasks_queue_t *q)
{
    /* IMPORTANT: We chose not to free the queues to simplify the
     * termination of the program (and make debugging less complex) */
    
    /* free(q->task_buffer); */
    /* free(q); */
}


void enqueue_task(tasks_queue_t *q, task_t *t)
{
    if (q->index == q->task_buffer_size) {
        int increase = q->task_buffer_size / 4;
        q->task_buffer_size += increase;
        q->task_buffer = (task_t**) realloc(q->task_buffer, sizeof(task_t*) * q->task_buffer_size);
    }

    q->task_buffer[q->index] = t;
    q->index++;
}


task_t* dequeue_task(tasks_queue_t *q)
{
    task_t *t = q->task_buffer[q->index-1];
    q->index--;

    return t;
}

