#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "tasks.h"
#include "debug.h"

pthread_t thread_pool[THREAD_COUNT];
tasks_queue_t *tqueue_pool[THREAD_COUNT];


void* thread_routine(void* tqueue_pointer) {
    tasks_queue_t *tqueue = (tasks_queue_t *) tqueue_pointer;
    while (true)
    {
        active_task = dequeue_task(tqueue);

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
    enqueue_task(tqueue_pool[sys_state.last_dispatched_queue], t);
    sys_state.last_dispatched_queue = (sys_state.last_dispatched_queue + 1) % THREAD_COUNT;
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
    if(t->task_dependency_done == t->task_dependency_count && t->status == WAITING){
        t->status = READY;
        dispatch_task(t);
    }
#endif
}
