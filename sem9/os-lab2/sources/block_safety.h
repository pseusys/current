#ifndef OS_LAB2_SOURCES_BLOCK_SAFETY_H
#define OS_LAB2_SOURCES_BLOCK_SAFETY_H

#include <stdbool.h>

#include "mem_alloc_types.h"

void check_no_leaks(void *heap_start, mb_free_t *first_free);

char* validate_allocated_block(mb_allocated_t *p, mb_free_t *first_free, void *heap_start);
char* validate_free_block(mb_free_t *p, mb_free_t *first_free, void *heap);

#endif // OS_LAB2_SOURCES_BLOCK_SAFETY_H
