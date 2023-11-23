#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "tasks.h"
#include "debug.h"

pthread_t thread_pool[THREAD_COUNT];
tasks_queue_t *tqueue_pool[THREAD_COUNT];

__thread tasks_queue_t *tqueue;


void* thread_routine(void* tqueue_pointer) {
    tqueue = (tasks_queue_t *) tqueue_pointer;
    while (true)
    {
        pthread_mutex_lock(&sys_state.stealing_mutex);
        while (true) {
            if (tqueue->length == 0) {
                PRINT_DEBUG(10, "Task will be stolen!\n");
                for (int i = 0; i < THREAD_COUNT; i++)
                    if (tqueue_pool[i]->length != 0) {
                        active_task = remove_task(tqueue_pool[i]);
                        PRINT_DEBUG(10, "Task will be stolen from queue: %d\n", i);
                        break;
                    }
            } else {
                active_task = pop_task(tqueue);
                if (tqueue->length > 1) {
                    PRINT_DEBUG(10, "Tasks %p in queue: %d; waking a helper thread up!\n", (void *) tqueue, tqueue->length);
                    pthread_cond_signal(&sys_state.stealing_condition);
                }
            }
            if (active_task == NULL) 
                pthread_cond_wait(&sys_state.stealing_condition, &sys_state.stealing_mutex);
            else break;
        }
        pthread_mutex_unlock(&sys_state.stealing_mutex);

        task_return_value_t ret = exec_task(active_task);

        pthread_mutex_lock(&sys_state.system_mutex);
        if (ret == TASK_COMPLETED){
            terminate_task(active_task);
            if (sys_state.task_created_counter == ++sys_state.task_finished_counter)
                pthread_cond_signal(&sys_state.system_finished);
        }
#ifdef WITH_DEPENDENCIES
        else{
            active_task->status = WAITING;
            PRINT_DEBUG(10, "Task suspended: %u\n", active_task->task_id);
            task_check_runnable(active_task);
        }
#endif
        pthread_mutex_unlock(&sys_state.system_mutex);

        active_task = NULL;
    }
    return NULL;
}


void create_queues(void)
{
    for (int i = 0; i < THREAD_COUNT; i++)
        tqueue_pool[i] = create_tasks_queue();
    tqueue = tqueue_pool[0];
}

void delete_queues(void)
{
    for (int i = 0; i < THREAD_COUNT; i++)
        free_tasks_queue(tqueue_pool[i]);
}    

void create_thread_pool(void)
{
    for (int i = 0; i < THREAD_COUNT; i++) {
        if (pthread_create(&thread_pool[i], NULL, thread_routine, tqueue_pool[i]) != 0){
            fprintf(stderr, "ERROR: failed to create thread %u\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void dispatch_task(task_t *t)
{
    push_task(tqueue, t);
    if (tqueue->length > 1 || active_task == NULL) pthread_cond_signal(&sys_state.stealing_condition);
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

    // free(t);
}

void task_check_runnable(task_t *t)
{
#ifdef WITH_DEPENDENCIES
    if(t->task_dependency_done == t->task_dependency_count && t->status == WAITING){
        t->status = READY;
        dispatch_task(t);
    }
#endif
}
