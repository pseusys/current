#ifndef __PARALLEL_FOR_H__
#define __PARALLEL_FOR_H__

#include "tasks.h"

typedef struct for_inputs{
    long int begin_index;
    long int end_index;
    long int incr;
} for_inputs_t;

typedef void (*reduce_function_t)(void *, void*, unsigned int);

void parallel_for_with_reduction(task_routine_t f, long int begin_index, long int end_index, long int incr, long int nb_blocks, void *output, size_t output_size, reduce_function_t rf);

#endif
