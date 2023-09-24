#ifndef   	_MEM_ALLOC_SIM_H_
#define   	_MEM_ALLOC_SIM_H_

#include <stdlib.h>

#include "sim_alloc.h"

/* Allocator functions, to be implemented in mem_alloc.c */
void memory_init_sim(size_t pool_size, policy_t policy, unsigned alignment);
void *memory_alloc(size_t size);
void memory_free(void *p);
size_t memory_get_allocated_block_size(void *addr);

/* display function */
void print_mem_state(void);

/* Logging functions */
void print_alloc_info(void *addr, int size);
void print_free_info(void *addr);
void print_error_alloc(int size);


#endif
