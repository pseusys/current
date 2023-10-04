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

#include "basic_safety.h"

/* pointer to the beginning of the memory region to manage */
void *heap_start;

/* Pointer to the first free block in the heap */
mb_free_t *first_free; 


#define ULONG(x)((long unsigned int)(x))

#if defined(FIRST_FIT)

/* TODO: code specific to the FIRST FIT allocation policy can be
 * inserted here */

/* You can define here functions that will be compiled only if the
 * symbol FIRST_FIT is defined, that is, only if the selected policy
 * is FF */


#elif defined(BEST_FIT)

/* TODO: code specific to the BEST FIT allocation policy can be
 * inserted here */

#elif defined(NEXT_FIT)

/* TODO: code specific to the NEXT FIT allocation policy can be
 * inserted here */

#endif


void run_at_exit(void)
{
    print_mem_state();
    check_no_leaks(heap_start, first_free);
    my_munmap(heap_start, MEM_POOL_SIZE);
}

void memory_init(void)
{
    /* register the function that will be called when the programs exits */
    atexit(run_at_exit);

    // Allocate some space for heap
    heap_start = my_mmap(MEM_POOL_SIZE);
    first_free = (mb_free_t *) heap_start;
    first_free->size = MEM_POOL_SIZE;
    first_free->next_block = NULL;
    print_info();
}

void *memory_alloc(size_t size)
{
    mb_free_t *next_empty_block = first_free, *previous_empty_block = NULL;
    while (next_empty_block != NULL) {
        if (next_empty_block->size < size + sizeof(mb_allocated_t)) {
            previous_empty_block = next_empty_block;
            next_empty_block = next_empty_block->next_block;
            continue;
        }

        mb_allocated_t *alloc_block = NULL;

        if (next_empty_block->size < size + sizeof(mb_allocated_t) + sizeof(mb_free_t)) {
            if (previous_empty_block != NULL) previous_empty_block->next_block = next_empty_block->next_block;
            else first_free = next_empty_block->next_block;
            alloc_block = (mb_allocated_t *) next_empty_block;
            alloc_block->size = next_empty_block->size - sizeof(mb_allocated_t);

        } else {
            mb_free_t *new_empty = (mb_free_t *) (((byte *) next_empty_block) + sizeof(mb_allocated_t) + size);
            if (previous_empty_block != NULL) previous_empty_block->next_block = new_empty;
            else first_free = new_empty;
            new_empty->next_block = next_empty_block->next_block;
            new_empty->size = next_empty_block->size - sizeof(mb_allocated_t) - size;
            alloc_block = (mb_allocated_t *) next_empty_block;
            alloc_block->size = size;
        }

        byte *address = ((byte *) alloc_block) + sizeof(mb_allocated_t);
        print_alloc_info(address, size);
        return address;
    }

    print_alloc_error(size);
    exit(0);
}

void memory_free(void *p)
{
    mb_allocated_t *alloc_block = (mb_allocated_t *) (((byte *) p) - sizeof(mb_allocated_t));
    char *free_error = check_freeing_block(alloc_block, first_free, heap_start);
    if (free_error != NULL) {
        printf("Error freeing memory block at %ld: %s!\n", (size_t) p - (size_t) heap_start, free_error);
        return;
    }

    size_t alloc_block_size = alloc_block->size + sizeof(mb_allocated_t);
    mb_free_t *freeing_block = (mb_free_t *) alloc_block;

    mb_free_t *next_empty_block = first_free, *previous_empty_block = NULL;
    while (((byte *) next_empty_block) < ((byte *) alloc_block) && next_empty_block != NULL) {
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
    print_free_info(p);
}

size_t memory_get_allocated_block_size(void *addr)
{
    mb_allocated_t *alloc_block = (mb_allocated_t *) (((byte *) addr) - sizeof(mb_allocated_t));
    return alloc_block->size - sizeof(mb_allocated_t);
}


void print_mem_state(void)
{
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

void print_free_info(void *addr){
    if(addr){
        fprintf(stderr, "FREE  at : %lu \n", ULONG((char*)addr - (char*)heap_start));
    }
    else{
        fprintf(stderr, "FREE  at : %lu \n", ULONG(0));
    }
    
}

void print_alloc_info(void *addr, int size){
  if(addr){
    fprintf(stderr, "ALLOC at : %lu (%d byte(s))\n", 
	    ULONG((char*)addr - (char*)heap_start), size);
  }
  else{
    fprintf(stderr, "Warning, system is out of memory\n"); 
  }
}

void print_alloc_error(int size) 
{
    fprintf(stderr, "ALLOC error : can't allocate %d bytes\n", size);
}


#ifdef MAIN
int main(int argc, char **argv){

  /* The main can be changed, it is *not* involved in tests */
  memory_init();
  print_info();
  int i ; 
  for( i = 0; i < 10; i++){
    char *b = memory_alloc(rand()%8);
    memory_free(b);
  }

  char * a = memory_alloc(15);
  memory_free(a);


  a = memory_alloc(10);
  memory_free(a);

  fprintf(stderr,"%lu\n",(long unsigned int) (memory_alloc(9)));
  return EXIT_SUCCESS;
}
#endif 
