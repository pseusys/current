#include "mem_alloc.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include "mem_alloc_types.h"
#include "my_mmap.h"

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
    fprintf(stderr,"YEAH B-)\n");
    my_munmap(heap_start, MEM_POOL_SIZE);
}




void memory_init(void)
{
    /* register the function that will be called when the programs exits */
    atexit(run_at_exit);

    heap_start = my_mmap(MEM_POOL_SIZE);
    first_free = (mb_free_t *) heap_start;
    first_free->size = MEM_POOL_SIZE;
    first_free->next_block = NULL;
    first_free->previous_block = NULL;
}

void *memory_alloc(size_t size)
{
    mb_free_t *empty_block = first_free;
    while (empty_block != NULL) {
        if (empty_block->size == size + sizeof(mb_allocated_t)) {
            empty_block->next_block->previous_block = empty_block->previous_block;
            empty_block->previous_block->next_block = empty_block->next_block;
            mb_allocated_t *alloc_block = (mb_allocated_t *) empty_block;
            alloc_block->size = size;
            return ((void *) alloc_block) + sizeof(mb_allocated_t);
        } else if (next_block->size >= size + sizeof(mb_allocated_t) + sizeof(mb_free_t)) {
            void *empty_difference = empty_block->size - size - sizeof(mb_allocated_t);
            empty_block->size = (size_t) empty_difference;
        }
        empty_block = empty_block->next_block;
    }
    exit(0);
}

void memory_free(void *p)
{

    /* TODO: insert your code here */

    /* TODO : don't forget to call the function print_free_info()
     * appropriately */

}

size_t memory_get_allocated_block_size(void *addr)
{

    /* TODO: insert your code here */

    return 0;
}


void print_mem_state(void)
{
    /* TODO: insert your code here */
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
