#ifndef OS_LAB2_SOURCES_BASIC_SAFETY_H
#define OS_LAB2_SOURCES_BASIC_SAFETY_H

#include <stdbool.h>

#include "mem_alloc_types.h"

void check_no_leaks(void *heap_start, mb_free_t *first_free);

char* check_freeing_block(mb_allocated_t *p, mb_free_t *first_free, void *heap_start);

#endif // OS_LAB2_SOURCES_BASIC_SAFETY_H
