#ifndef   _MY_MMAP_H_
#define   _MY_MMAP_H_

#include <sys/mman.h>

/* 
 * Allocates a region of virtual memory
 * and returns a pointer to the start of this region
 * (or NULL if the allocation failed).
 * The returned address is a multiple of MEM_ALIGNMENT.
 */
void *my_mmap(size_t size);

/*
 * Frees a region of virtual memory.
 */
int my_munmap(void *addr, size_t length); 

#endif      /* !_MY_MMAP_H_ */
