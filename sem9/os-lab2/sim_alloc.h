#ifndef   	_SIM_ALLOC_H_
#define   	_SIM_ALLOC_H_

#include <stdlib.h>

#define MAX_BLOCK 1024

typedef enum policy{
    BF,
    WF,
    FF,
    NF
} policy_t;

/* allocated blocks*/
typedef struct simulator_block{
    int index;
    int size;
    int next;
}simulator_block_t;

typedef struct simulator_state{
    char *memory;
    size_t memory_size;
    unsigned mem_align_value;
    policy_t alloc_policy;
    int start_searching_index; /* use for next fit */
    int nb_blocks;
    int first_block;
    simulator_block_t stack[MAX_BLOCK];
    size_t free_header_size;
    size_t alloc_header_size;
} simulator_state_t;
    


/* Allocator sim functions*/
simulator_state_t* sim_init(size_t pool_size, policy_t policy, unsigned alignement, size_t free_header_size, size_t alloc_header_size);
void *sim_alloc(simulator_state_t *state, size_t size);
void sim_free(simulator_state_t *state, void *p);
size_t sim_get_allocated_block_size(simulator_state_t *state, void *addr);

#endif 	    /* !_SIM_ALLOC_H_ */
