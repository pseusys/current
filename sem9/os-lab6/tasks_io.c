#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "tasks.h"

pthread_mutex_t param_usage_mutex = PTHREAD_MUTEX_INITIALIZER;

/* !!! None of these functions need to be modified until stage B2 */

/* Appends 'e' to the list of params */
static void param_list_append(task_param_list_t *list, task_param_t *e)
{   
    if (list->next == NULL){
        list->head = list->next = e;
        return;
    }

    task_param_t *iter = list->next;
    
    while(iter->next != NULL){
        iter = iter->next;
    }

    iter->next = e;
}

/* Retrieves the first element from the list of params */
static task_param_t* param_list_remove_first(task_param_list_t *list)
{
    task_param_t *elem = list->next;

    assert(elem != NULL);   
     
    list->next = elem->next;
    
    return elem;
}

/* Allocates a region of size 's' and appends it as param to the list
 * of params */
static task_param_t* attach_param(task_param_list_t *list, size_t s)
{
    task_param_t* param = malloc(sizeof(task_param_t));
    task_param_payload_t* payload = malloc(sizeof(task_param_payload_t));

    param->next = NULL;
    param->payload = payload;
    param->payload->uses = 1;
    param->payload->elem = malloc(s);

    param_list_append(list, param);

    return param;
}

#ifdef WITH_DEPENDENCIES
/* Creates a new parameter (without allocating a memory region) and
 * appends it to the list of params */
static task_param_t* attach_param_with_elem(task_param_list_t *list, task_param_payload_t* payload)
{
    task_param_t* param = malloc(sizeof(task_param_t));

    param->next = NULL;
    param->payload = payload;

    pthread_mutex_lock(&param_usage_mutex);
    param->payload->uses++;
    pthread_mutex_unlock(&param_usage_mutex);

    param_list_append(list, param);
    return param;
}
#endif

/* Retrieves one parameter from a list */
static task_param_payload_t* retrieve_param_payload(task_param_list_t *list)
{
    task_param_payload_t* payload = param_list_remove_first(list)->payload;

    pthread_mutex_lock(&param_usage_mutex);
    payload->uses--;
    pthread_mutex_unlock(&param_usage_mutex);

    return payload;
}

/* Allocates 's' bytes and attaches a new input parameter to task 't'*/
// ENTRYPOINT
void* attach_input(task_t *t, size_t s)
{
    task_param_t* param = attach_param(t->tstate.input_list, s);
    
    return param->payload->elem;
}

/* Allocates 's' bytes and attaches a new output parameter to task 't'*/
// ENTRYPOINT
void* attach_output(task_t *t, size_t s)
{
    task_param_t* param = attach_param(t->tstate.output_list, s);

#ifdef WITH_DEPENDENCIES
    if(active_task != NULL){
        /* the active task is interested in the outputs */
        attach_param_with_elem(active_task->tstate.output_from_dependencies_list, param->payload);
        return param->payload->elem;
    }
#endif
    
    return param->payload->elem;
}

/* Retrieves the next input parameter of task 't'*/
/* parameters are retrieved in the order they have been attached */
// ENTRYPOINT
void* retrieve_input(task_t *t)
{
    task_param_payload_t *payload = retrieve_param_payload(t->tstate.input_list);
    return payload->elem;
}

/* Retrieves the next output parameter of task 't'*/
/* parameters are retrieved in the order they have been attached */
// ENTRYPOINT
void* retrieve_output(task_t *t)
{
    task_param_payload_t *payload = retrieve_param_payload(t->tstate.output_list);
    return payload->elem;
}

#ifdef WITH_DEPENDENCIES
/* Retrieves an output paramter generated by a child task of task 't' */
// ENTRYPOINT
void* retrieve_output_from_dependencies(task_t *t)
{
    task_param_payload_t *payload = retrieve_param_payload(t->tstate.output_from_dependencies_list);
    return payload->elem;
}
#else
void* retrieve_output_from_dependencies(task_t *t)
{
    return NULL;
}
#endif


task_param_list_t *init_task_io_list()
{
    task_param_list_t *list = malloc(sizeof(task_param_list_t));
    list->head = list->next = NULL;
    return list;
}

void free_task_io_list(task_param_list_t *list)
{
    task_param_t *iter = list->head;
    while(iter != NULL) {
        task_param_t *next = iter->next;
        if (iter->payload->uses == 0) {
            free(iter->payload->elem);
            free(iter->payload);
        }
        free(iter);
        iter = next;
    }
    free(list);
}
