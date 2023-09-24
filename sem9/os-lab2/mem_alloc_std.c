/* #define _GNU_SOURCE */


#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "mem_alloc.h"
#include "mem_alloc_types.h"


static int __mem_alloc_init_flag=0;

/****************************************************************************/
/*
 * Workaround for the bootstrap.
 * This is required because dlsym (called in memory_init,
 * which is itself called during the first call to malloc)
 * invokes calloc (indirectly, through the _dlerror_run function).
 */

static int __mem_alloc_init_completed = 0;

/* in bytes */
#define BOOTSTRAP_BUFFER_SIZE 1024

#define NB_BOOTSTRAP_BUFFERS 10

typedef struct bbuf {
    uint8_t buf[BOOTSTRAP_BUFFER_SIZE];
} bootstrap_buf;

static bootstrap_buf bootstrap_buffers[NB_BOOTSTRAP_BUFFERS];
static uint8_t used_bootstrap_buffers[NB_BOOTSTRAP_BUFFERS]; /* booleans */

void init_bootstrap_buffers() {
    debug_printf("enter\n");
    int i;
    void* buf_addr;
    for (i = 0; i < NB_BOOTSTRAP_BUFFERS; i++) {
        used_bootstrap_buffers[i] = 0;
        buf_addr = &(bootstrap_buffers[i].buf);
        assert( ((unsigned long long)buf_addr) % MEM_ALIGNMENT == 0 );
        memset(buf_addr, 0, BOOTSTRAP_BUFFER_SIZE);
    }
    debug_printf("return\n");
}

void *handle_bootstrap_alloc(size_t size) {
    int i = 0;
    void * res;

    debug_printf("enter - size = %ld\n", size);
    assert(size <= BOOTSTRAP_BUFFER_SIZE);
    while (i < NB_BOOTSTRAP_BUFFERS) {
        if (!used_bootstrap_buffers[i]) {
            used_bootstrap_buffers[i] = 1;
            res = (void*)&(bootstrap_buffers[i].buf);
            break;
        }
        i++;
    }
    debug_printf("i = %d\n", i);
    assert(i < NB_BOOTSTRAP_BUFFERS);
    debug_printf("return %p\n", res);
    return res;
}

void handle_bootstrap_free(void *p) {
    int i = 0;
    debug_printf("enter - p = %p\n", p);
    while (i < NB_BOOTSTRAP_BUFFERS) {
        if (p == (void*)bootstrap_buffers[i].buf) {
            used_bootstrap_buffers[i] = 0;
            memset(&(bootstrap_buffers[i].buf), 0, BOOTSTRAP_BUFFER_SIZE);
            break;
        }
        i++;
    }
    assert(i < NB_BOOTSTRAP_BUFFERS);
    debug_printf("return\n");
}

/* returns a boolean */
int is_bootstrap_buffer(void *p) {
    int res = 0;
    debug_printf("enter - p = %p\n", p);
    if ((p >= (void*)(bootstrap_buffers[0].buf)) && (p < ((void*)(bootstrap_buffers[NB_BOOTSTRAP_BUFFERS-1].buf)+BOOTSTRAP_BUFFER_SIZE-1))) {
        res = 1;
    }
    debug_printf("return %d\n", res);
    return res;
}

/****************************************************************************/


void *malloc(size_t size){
  void *res;

  debug_printf("enter: size = %ld\n", size);

  if(!__mem_alloc_init_flag){
      __mem_alloc_init_flag = 1;
      init_bootstrap_buffers();
      memory_init();
      debug_printf("memory_init completed\n"); // FOR DEBUG ONLY
      //print_info();
      __mem_alloc_init_completed = 1;
  } else if (!__mem_alloc_init_completed) {
      res = handle_bootstrap_alloc(size);
      debug_printf("return = %p\n", res);
      return res;
  }  
  
  res = memory_alloc(size);
  debug_printf("return = %p\n", res);
  return res;
}

void free(void *p){
    debug_printf("enter: p = %p\n", p);

    if (is_bootstrap_buffer(p)) {
        handle_bootstrap_free(p);
        return;
    }

    if (p == NULL) return;
    memory_free(p);

    debug_printf("return\n");
}

#ifndef DISABLE_CALLOC_INTERPOSITION
void *calloc(size_t nmemb, size_t size)
{
    void *res;
    
    debug_printf("enter: nmemb = %ld, size = %ld\n", nmemb, size);

    if(!__mem_alloc_init_flag){
        __mem_alloc_init_flag = 1;
        init_bootstrap_buffers();
        memory_init();
        //print_info();
        __mem_alloc_init_completed = 1;
    } else if (!__mem_alloc_init_completed) {
      return handle_bootstrap_alloc(size);
    }

#ifdef CALLOC_INTERPOSITION_PASSTROUGH
    assert(o_calloc != NULL);
    res = o_calloc(nmemb, size);
    debug_printf("return = %p\n", res);
    return res;
#endif

    res = memory_alloc(size*nmemb);
    if (res != NULL) {
        explicit_bzero(res, size);
    }

    debug_printf("return = %p\n", res);
    return res;
}
#endif

void *realloc(void *ptr, size_t size){

    void *new;
    size_t old_size;
    void *res;

    debug_printf("enter: ptr = %p, size = %ld\n", ptr, size);

    /* This should not be needed if realloc is used properly but just in case ... */
    if(!__mem_alloc_init_flag){ 
        __mem_alloc_init_flag = 1;
        init_bootstrap_buffers();
        memory_init();
        printf("memory_init completed\n"); // FOR DEBUG ONLY
        //print_info();
        __mem_alloc_init_completed = 1;
    } else if (!__mem_alloc_init_completed) {
      assert(0); /* Support for bootstrap realloc not implemented */
    }

    if (ptr == NULL) { 
        res = memory_alloc(size); /* according to the specification (malloc man page) */
        debug_printf("return = %p\n", res);
        return res;
    }

    if ((size == 0) && (ptr != NULL)) { 
        memory_free(ptr); /* according to the specification (malloc man page) */
        res = NULL;
        debug_printf("return = %p\n", res);
        return res;
    }

#if (defined(DISABLE_FAST_POOL_ALLOC) || defined(DISABLE_STD_POOL_ALLOC) || defined(DISABLE_CALLOC_INTERPOSITION) || defined(CALLOC_INTERPOSITION_PASSTROUGH) )
    /* Note: we assume that the pointer is valid. */    
    if (find_pool_from_block_address(ptr) == -1) {
        /* 
         * The memory block was allocated from the "real/original" malloc heap.
         * So we directly forward the realloc request to it.
         */
        assert(o_realloc != NULL);
        res = o_realloc(ptr, size);
        debug_printf("return = %p\n", res);
        return res;
    }
#endif

    /*
     * The reallocation is naive/suboptimal (systematic copy) but does not require to
     * expose much of the allocator internals, nor to support realloc within the allocator.
     */
    new = memory_alloc(size);
    if (new == NULL) {
        /* The original block is left untouched, as required in the specification. */
        return NULL; 
    }
    old_size = memory_get_allocated_block_size(ptr);
    memcpy(new, ptr, old_size); /* works because the two areas do not overlap */
    free(ptr);
    debug_printf("return = %p\n", new);
    return new;
}
