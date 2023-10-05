#include "mem_alloc.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../my_mmap.h"
#include "block_safety.h"

#define ULONG(x)((long unsigned int)(x))


/* pointer to the beginning of the memory region to manage */
void *heap_start;

/* Pointer to the first free block in the heap */
mb_free_t *first_free; 


size_t align(size_t raw) {
    if (raw % MEM_ALIGNMENT == 0) return raw;
    else return ((raw / MEM_ALIGNMENT) + 1) * MEM_ALIGNMENT;
}

void check_empty_block(mb_free_t *empty_block) {
    size_t a_free = align(sizeof(mb_free_t));
    char *allocate_error = validate_free_block(empty_block, first_free, heap_start, (void *) ((size_t) empty_block + a_free), empty_block->size - a_free);
    if (allocate_error != NULL) {
        printf("Error checking free memory block at %ld: %s!\n", (size_t) empty_block - (size_t) heap_start, allocate_error);
        exit(EXIT_FAILURE);
    }
}

#if defined(FIRST_FIT)

void *memory_alloc(size_t size) {
    size_t a_size = align(size);
    size_t a_alloc = align(sizeof(mb_allocated_t));
    size_t a_free = align(sizeof(mb_free_t));

    mb_free_t *empty_block = first_free, *previous_empty_block = NULL;
    while (empty_block != NULL) {
        check_empty_block(empty_block);

        if (empty_block->size < a_size + a_alloc) {
            previous_empty_block = empty_block;
            empty_block = empty_block->next_block;
            continue;
        }

        mb_allocated_t *alloc_block = NULL;

        if (empty_block->size < a_size + a_alloc + a_free) {
            if (previous_empty_block != NULL) previous_empty_block->next_block = empty_block->next_block;
            else first_free = empty_block->next_block;
            alloc_block = (mb_allocated_t *) empty_block;
            alloc_block->size = empty_block->size - a_alloc;

        } else {
            mb_free_t *new_empty = (mb_free_t *) (((byte *) empty_block) + a_alloc + a_size);
            if (previous_empty_block != NULL) previous_empty_block->next_block = new_empty;
            else first_free = new_empty;
            new_empty->next_block = empty_block->next_block;
            new_empty->size = empty_block->size - a_alloc - a_size;
            alloc_block = (mb_allocated_t *) empty_block;
            alloc_block->size = size;
        }

        byte *address = ((byte *) alloc_block) + a_alloc;
        print_alloc_info(address, size);
        return address;
    }

    print_alloc_error(size);
    exit(EXIT_SUCCESS);
}

#elif defined(BEST_FIT)

void *memory_alloc(size_t size) {
    size_t a_size = align(size);
    size_t a_alloc = align(sizeof(mb_allocated_t));
    size_t a_free = align(sizeof(mb_free_t));

    mb_free_t *best_empty_block = NULL, *best_previous_empty_block = NULL;
    mb_free_t *empty_block = first_free, *previous_empty_block = NULL;
    while (empty_block != NULL) {
        check_empty_block(empty_block);
        if (empty_block->size >= a_size + a_alloc) {
            if (best_empty_block == NULL || empty_block->size < best_empty_block->size) {
                best_empty_block = empty_block;
                best_previous_empty_block = previous_empty_block;
            }
        }
        previous_empty_block = empty_block;
        empty_block = empty_block->next_block;
    }

    if (best_empty_block == NULL) {
        print_alloc_error(size);
        exit(EXIT_SUCCESS);
    }

    mb_allocated_t *alloc_block = NULL;

    if (best_empty_block->size < a_size + a_alloc + a_free) {
        if (best_previous_empty_block != NULL) best_previous_empty_block->next_block = best_empty_block->next_block;
        else first_free = best_empty_block->next_block;
        alloc_block = (mb_allocated_t *) best_empty_block;
        alloc_block->size = best_empty_block->size - a_alloc;

    } else {
        mb_free_t *new_empty = (mb_free_t *) (((byte *) best_empty_block) + a_alloc + a_size);
        if (best_previous_empty_block != NULL) best_previous_empty_block->next_block = new_empty;
        else first_free = new_empty;
        new_empty->next_block = best_empty_block->next_block;
        new_empty->size = best_empty_block->size - a_alloc - a_size;
        alloc_block = (mb_allocated_t *) best_empty_block;
        alloc_block->size = size;
    }

    byte *address = ((byte *) alloc_block) + a_alloc;
    print_alloc_info(address, size);
    return address;
}

#elif defined(WORST_FIT)

void *memory_alloc(size_t size) {
    size_t a_size = align(size);
    size_t a_alloc = align(sizeof(mb_allocated_t));
    size_t a_free = align(sizeof(mb_free_t));

    mb_free_t *worst_empty_block = NULL, *worst_previous_empty_block = NULL;
    mb_free_t *empty_block = first_free, *previous_empty_block = NULL;
    while (empty_block != NULL) {
        check_empty_block(empty_block);
        if (empty_block->size >= a_size + a_alloc) {
            if (worst_empty_block == NULL || empty_block->size > worst_empty_block->size) {
                worst_empty_block = empty_block;
                worst_previous_empty_block = previous_empty_block;
            }
        }
        previous_empty_block = empty_block;
        empty_block = empty_block->next_block;
    }

    if (worst_empty_block == NULL) {
        print_alloc_error(size);
        exit(EXIT_SUCCESS);
    }

    mb_allocated_t *alloc_block = NULL;

    if (worst_empty_block->size < a_size + a_alloc + a_free) {
        if (worst_previous_empty_block != NULL) worst_previous_empty_block->next_block = worst_empty_block->next_block;
        else first_free = worst_empty_block->next_block;
        alloc_block = (mb_allocated_t *) worst_empty_block;
        alloc_block->size = worst_empty_block->size - a_alloc;

    } else {
        mb_free_t *new_empty = (mb_free_t *) (((byte *) worst_empty_block) + a_alloc + a_size);
        if (worst_previous_empty_block != NULL) worst_previous_empty_block->next_block = new_empty;
        else first_free = new_empty;
        new_empty->next_block = worst_empty_block->next_block;
        new_empty->size = worst_empty_block->size - a_alloc - a_size;
        alloc_block = (mb_allocated_t *) worst_empty_block;
        alloc_block->size = a_alloc;
    }

    byte *address = ((byte *) alloc_block) + a_alloc;
    print_alloc_info(address, size);
    return address;
}

#elif defined(NEXT_FIT)

mb_free_t *next_free; 

void *memory_alloc(size_t size) {
    size_t a_size = align(size);
    size_t a_alloc = align(sizeof(mb_allocated_t));
    size_t a_free = align(sizeof(mb_free_t));

    if (next_free == NULL) next_free = first_free;

    mb_free_t *empty_block = next_free, *previous_empty_block = NULL;
    while (empty_block != NULL) {
        check_empty_block(empty_block);

        if (empty_block->size < a_size + a_alloc) {
            previous_empty_block = empty_block;
            empty_block = empty_block->next_block;
            continue;
        }

        mb_allocated_t *alloc_block = NULL;

        if (empty_block->size < a_size + a_alloc + a_free) {
            if (previous_empty_block != NULL) previous_empty_block->next_block = empty_block->next_block;
            else first_free = empty_block->next_block;
            alloc_block = (mb_allocated_t *) empty_block;
            alloc_block->size = empty_block->size - a_alloc;
            next_free = empty_block->next_block;

        } else {
            mb_free_t *new_empty = (mb_free_t *) (((byte *) empty_block) + a_alloc + a_size);
            if (previous_empty_block != NULL) previous_empty_block->next_block = new_empty;
            else first_free = new_empty;
            new_empty->next_block = empty_block->next_block;
            new_empty->size = empty_block->size - a_alloc - a_size;
            alloc_block = (mb_allocated_t *) empty_block;
            alloc_block->size = size;
            next_free = new_empty;
        }

        byte *address = ((byte *) alloc_block) + a_alloc;
        print_alloc_info(address, size);
        return address;
    }

    print_alloc_error(size);
    exit(EXIT_SUCCESS);
}

#endif


void run_at_exit(void) {
    print_mem_state();
    check_no_leaks(heap_start, first_free);
    my_munmap(heap_start, MEM_POOL_SIZE);
}

void memory_init(void) {
    /* register the function that will be called when the programs exits */
    atexit(run_at_exit);

    // Allocate some space for heap
    heap_start = my_mmap(MEM_POOL_SIZE);
    memset((void *) heap_start, 0, MEM_POOL_SIZE);
    first_free = (mb_free_t *) heap_start;
    first_free->size = MEM_POOL_SIZE;
    first_free->next_block = NULL;
    print_info();
}

void memory_free(void *p) {
    size_t a_alloc = align(sizeof(mb_allocated_t));
    size_t a_free = align(sizeof(mb_free_t));

    mb_allocated_t *alloc_block = (mb_allocated_t *) (((byte *) p) - a_alloc);
    char *free_error = validate_allocated_block(alloc_block, first_free, heap_start);
    if (free_error != NULL) {
        printf("Error freeing memory block at %ld: %s!\n", (size_t) p - (size_t) heap_start, free_error);
        return;
    }

    size_t alloc_block_size = align(alloc_block->size) + a_alloc;
    mb_free_t *freeing_block = (mb_free_t *) alloc_block;

    mb_free_t *next_empty_block = first_free, *previous_empty_block = NULL;
    while (((byte *) next_empty_block) < ((byte *) alloc_block) && next_empty_block != NULL) {
        check_empty_block(next_empty_block);
        previous_empty_block = next_empty_block;
        next_empty_block = next_empty_block->next_block;
    }

    mb_free_t *next_empty_block_next;
    size_t next_empty_block_size;

    if (previous_empty_block == NULL) first_free = freeing_block;
    else previous_empty_block->next_block = freeing_block;
    if (next_empty_block == NULL) freeing_block->next_block = NULL;
    else {
        next_empty_block_next = next_empty_block->next_block;
        next_empty_block_size = next_empty_block->size;
        freeing_block->next_block = next_empty_block;
    }

    if (previous_empty_block != NULL && ((byte *) previous_empty_block) + previous_empty_block->size == (byte *) freeing_block) {
        previous_empty_block->next_block = next_empty_block;
        alloc_block_size += previous_empty_block->size;
        freeing_block = previous_empty_block;
    }
    if (next_empty_block != NULL && ((byte *) freeing_block) + alloc_block_size == (byte *) next_empty_block) {
        freeing_block->next_block = next_empty_block_next;
        alloc_block_size += next_empty_block_size;
    }

    freeing_block->size = alloc_block_size;
    memset((void *) ((size_t) freeing_block + a_free), 0, freeing_block->size - a_free);
    print_free_info(p);
}

size_t memory_get_allocated_block_size(void *addr) {
    mb_allocated_t *alloc_block = (mb_allocated_t *) (((byte *) addr) - sizeof(mb_allocated_t));
    return alloc_block->size - sizeof(mb_allocated_t);
}


void print_mem_state(void) {
    printf("Free heap blocks:\n");
    mb_free_t *empty_block = first_free;
    while (empty_block != NULL) {
        if (empty_block->next_block != NULL)
            printf("\tBlock at %ld of size %ld, next %ld\n", ULONG((char*) empty_block - (char*) heap_start), empty_block->size, ULONG((char*) empty_block->next_block - (char*) heap_start));
        else
            printf("\tLast block at %ld of size %ld\n", ULONG((char*) empty_block - (char*) heap_start), empty_block->size);
        empty_block = empty_block->next_block;
    }
}


void print_info(void) {
    fprintf(stderr, "Memory : [%lu %lu] (%lu bytes)\n", (long unsigned int) heap_start, (long unsigned int) ((char*)heap_start+MEM_POOL_SIZE), (long unsigned int) (MEM_POOL_SIZE));
}

void print_free_info(void *addr) {
    if(addr){
        fprintf(stderr, "FREE  at : %lu \n", ULONG((char*)addr - (char*)heap_start));
    }
    else{
        fprintf(stderr, "FREE  at : %lu \n", ULONG(0));
    }
    
}

void print_alloc_info(void *addr, int size) {
  if(addr){
    fprintf(stderr, "ALLOC at : %lu (%d byte(s))\n", 
	    ULONG((char*)addr - (char*)heap_start), size);
  }
  else{
    fprintf(stderr, "Warning, system is out of memory\n"); 
  }
}

void print_alloc_error(int size) {
    fprintf(stderr, "ALLOC error : can't allocate %d bytes\n", size);
}
