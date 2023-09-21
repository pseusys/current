#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "./mem_alloc_types.h"
#include "./mem_alloc_sim.h"
#include "./sim_alloc.h"

simulator_state_t *sim_state;


void memory_init_sim(size_t pool_size, policy_t policy, unsigned alignment)
{

    sim_state = sim_init(pool_size, policy, alignment, sizeof(mb_free_t), sizeof(mb_allocated_t));
}


    
void *memory_alloc(size_t size)
{
    return sim_alloc(sim_state, size);
}

void memory_free(void *p)
{
    sim_free(sim_state, p);
}

void print_alloc_info(void *addr, int size)
{
}

void print_free_info(void *addr)
{
}


void print_alloc_error(int size)
{
    fprintf(stderr, "ALLOC error : can't allocate %d bytes\n", size);
}
