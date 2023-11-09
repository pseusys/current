#include <stdio.h>
#include <stdlib.h>

#include "parallel_for.h"
#include "tasks.h"

void parallel_for_with_reduction(task_routine_t f, long int begin_index, long int end_index, long int incr, long int nb_blocks, void *output, size_t output_size, reduce_function_t rf)
{
    long int range = end_index - begin_index;

    if(range <= 0 || range % incr != 0 || range/incr % nb_blocks !=0){
        fprintf(stderr, "ERROR: Not correctly formed parallel loop -- start: %ld  end: %ld  incr: %ld  blocks: %ld", begin_index, end_index, incr, nb_blocks);
        exit(EXIT_FAILURE);
    }

    long int nb_iter = range/incr;

    long int block_size = nb_iter / nb_blocks;

    task_t *t;

    void* outs= malloc(output_size * nb_blocks);

    for(long int i=0; i <nb_blocks; i++){
        t = create_task(f);
        for_inputs_t *inputs = attach_input(t, sizeof(for_inputs_t));

        inputs->begin_index = begin_index + i * block_size * incr;
        inputs->end_index = begin_index + ((i+1) * block_size) * incr;
        inputs->incr = incr;
        void **out = attach_output(t, sizeof(void*));
        *out = ((char*)outs) + i * output_size;
        
        submit_task(t);
    }

    task_waitall();
    
    rf(outs, output, nb_blocks);

    free(outs);
}
