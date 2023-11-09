#include <stdio.h>
#include <pthread.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "debug.h"

bool ready_to_terminate = false;
int threads_busy = 0;

tasks_queue_t *tqueue = NULL;

pthread_t thread_pool[THREAD_COUNT];
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_finished = PTHREAD_COND_INITIALIZER;


void* thread_routine(void* arg) {
    while (true)
    {
        pthread_mutex_lock(&queue_mutex);
        while (tqueue->index == 0)
            pthread_cond_wait(&queue_not_empty, &queue_mutex);

        threads_busy++;
        ready_to_terminate = false;
        task_t *active_task = dequeue_task(tqueue);
        task_return_value_t ret = exec_task(active_task);
        if (ret == TASK_COMPLETED){
            terminate_task(active_task);
        }
#ifdef WITH_DEPENDENCIES
        else{
            active_task->status = WAITING;
        }
#endif
        threads_busy--;
        ready_to_terminate = (threads_busy == 0) && (tqueue->index == 0);

        pthread_cond_signal(&queue_not_full);
        pthread_cond_signal(&queue_finished);
        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}


void create_queues(void)
{
    tqueue = create_tasks_queue();
}

void delete_queues(void)
{
    free_tasks_queue(tqueue);
}    

void create_thread_pool(void)
{

    return ;
}

void dispatch_task(task_t *t)
{
    pthread_mutex_lock(&queue_mutex);
    while (tqueue->index == tqueue->task_buffer_size)
        pthread_cond_wait(&queue_not_full, &queue_mutex);

    enqueue_task(tqueue, t);

    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_mutex);
}

task_t* get_task_to_execute(void)
{
    return dequeue_task(tqueue);
}

unsigned int exec_task(task_t *t)
{
    t->step++;
    t->status = RUNNING;

    PRINT_DEBUG(10, "Execution of task %u (step %u)\n", t->task_id, t->step);
    
    unsigned int result = t->fct(t, t->step);
    
    return result;
}

void terminate_task(task_t *t)
{
    t->status = TERMINATED;
    
    PRINT_DEBUG(10, "Task terminated: %u\n", t->task_id);

#ifdef WITH_DEPENDENCIES
    if(t->parent_task != NULL){
        task_t *waiting_task = t->parent_task;
        waiting_task->task_dependency_done++;
        
        task_check_runnable(waiting_task);
    }
#endif

}

void task_check_runnable(task_t *t)
{
#ifdef WITH_DEPENDENCIES
    if(t->task_dependency_done == t->task_dependency_count){
        t->status = READY;
        dispatch_task(t);
    }
#endif
}
