#include <stdio.h>
#include <stdlib.h>

#include "tasks_queue.h"


tasks_queue_t* create_tasks_queue(void)
{
    tasks_queue_t *q = (tasks_queue_t*) malloc(sizeof(tasks_queue_t));

    q->head = NULL;
    q->tail = NULL;
    q->length = 0;

    pthread_mutex_init(&q->queue_mutext, NULL);
    pthread_cond_init(&q->queue_not_empty, NULL);

    return q;
}


void free_tasks_queue(tasks_queue_t *q)
{
    tasks_queue_block_t *current = q->head;
    while (current != NULL) {
        tasks_queue_block_t *next = current->next;
        if (current->task != NULL) free(current->task);
        free(current);
        current = next;
    }
    free(q);
}


void push_task(tasks_queue_t *q, task_t *t)
{
    pthread_mutex_lock(&q->queue_mutext);

    tasks_queue_block_t *block = (tasks_queue_block_t*) malloc(sizeof(tasks_queue_block_t));

    block->next = NULL;
    block->previous = q->tail;
    block->task = t;

    if (q->tail != NULL) q->tail->next = block;
    else q->head = block;
    q->tail = block;
    q->length++;

    pthread_cond_signal(&q->queue_not_empty);
    pthread_mutex_unlock(&q->queue_mutext);
}


task_t* pop_task(tasks_queue_t *q)
{
    pthread_mutex_lock(&q->queue_mutext);
    while (q->length == 0)
        pthread_cond_wait(&q->queue_not_empty, &q->queue_mutext);

    task_t *t = q->tail->task;

    tasks_queue_block_t *new_tail = q->tail->previous;
    free(q->tail);
    if (new_tail != NULL) new_tail->next = NULL;
    else q->head = new_tail;
    q->tail = new_tail;
    q->length--;

    pthread_mutex_unlock(&q->queue_mutext);

    return t;
}

task_t* remove_task(tasks_queue_t *q)
{
    pthread_mutex_lock(&q->queue_mutext);
    while (q->length == 0)
        pthread_cond_wait(&q->queue_not_empty, &q->queue_mutext);

    task_t *t = q->head->task;

    tasks_queue_block_t *new_head = q->head->next;
    free(q->head);
    if (new_head != NULL) new_head->previous = NULL;
    else q->tail = new_head;
    q->head = new_head;
    q->length--;

    pthread_mutex_unlock(&q->queue_mutext);

    return t;
}
