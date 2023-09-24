#ifndef   	_MEM_ALLOC_H_
#define   	_MEM_ALLOC_H_

#include <stdlib.h>


/* Allocator functions, to be implemented in mem_alloc.c */
void memory_init(void);
void *memory_alloc(size_t size);
void memory_free(void *p);
size_t memory_get_allocated_block_size(void *addr);


/////////////////////////////////////////////////////////
/* Functions for testing and debugging: */

/* Display function */
void print_mem_state(void); 

/* Function called upon process termination */
void run_at_exit(void);

/* Trace functions: */

/* 
 * This trace function must be called upon a successful block allocation.
 * addr = address of the allocated block payload
 * size = size of the payload
 */
void print_alloc_info(void *addr, int size);

/*
 * This trace function must be called upon a block deallocation.
 * addr = address of the allocated block payload
 */
void print_free_info(void *addr);

/*
 * This trace function must be called upon a FAILED block allocation.
 * size = size of the payload
 */
void print_alloc_error(int size);


/////////////////////////////////////////////////////////

/* Pointers to the original malloc functions */
extern void* (*o_malloc)(size_t);
extern void (*o_free)(void *);
extern void* (*o_realloc)(void*, size_t);
extern void* (*o_calloc)(size_t, size_t);

/////////////////////////////////////////////////////////

/* Output stream for debug (stdout or stderr) */
#define DEBUG_STREAM stderr

/*
 * This function works like printf but: 
 * - uses DEBUG_STREAM for the output
 * - can be enabled (-DDEBUG=1) or disabled (-DDEBUG=0) via the Makefile
 */
#define debug_printf(fmt, ...) \
            do { if (DEBUG) fprintf(DEBUG_STREAM, "%s:%d:%s(): " fmt, \
                                    __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
            } while (0)

#endif 	    /* !_MEM_ALLOC_H_ */
