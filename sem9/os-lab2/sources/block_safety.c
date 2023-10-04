#include <stdlib.h>
#include <stdio.h>

#include "block_safety.h"

void check_no_leaks(void *heap_start, mb_free_t *first_free) {
    mb_free_t *expected_first_free = (mb_free_t *) heap_start;
    bool expected_size = expected_first_free->size == MEM_POOL_SIZE;
    bool expected_list = first_free == expected_first_free;
    bool expected_next = first_free->next_block == NULL;

    if (!expected_size || !expected_next || !expected_list) {
        printf("WARNING! Memory leaks detected:\n");
        if (!expected_size || !expected_list) printf("\tSome memory still allocated!\n");
        if (!expected_next) printf("\tFragmented memory blocks allocated!\n");

        mb_free_t *empty_block = first_free, *previous_empty = expected_first_free;
        int occupied_memory = ((size_t) empty_block) - ((size_t) previous_empty);
        while (empty_block->next_block != NULL) {
            previous_empty = empty_block;
            empty_block = empty_block->next_block;
            occupied_memory += ((size_t) empty_block) - (((size_t) previous_empty) + previous_empty->size);
        }
        occupied_memory += (((size_t) heap_start) + MEM_POOL_SIZE) - (((size_t) empty_block) + empty_block->size);
        if (occupied_memory != 0) printf("\t%d heap bytes leaked!\n", occupied_memory);
    }
}

char* validate_allocated_block(mb_allocated_t *p, mb_free_t *first_free, void *heap_start) {
    size_t pointer_start = (size_t) p;
    size_t pointer_end = pointer_start + p->size;

    size_t heap = (size_t) heap_start;
    if (pointer_start < heap) return "pointer outside of the heap (lower)";

    mb_free_t *empty_block = first_free;
    while (empty_block != NULL) {
        size_t block_start = (size_t) empty_block;
        size_t block_end = block_start + empty_block->size;

        if (pointer_start >= block_start && pointer_start < block_end) return "pointer inside of a free block";
        if (pointer_end > block_start && pointer_end < block_end) return "allocated block continues to a free block";
        empty_block = empty_block->next_block;
    }

    if (pointer_end >= heap + MEM_POOL_SIZE) return "allocated block continues outside of the heap (higher)";
    return NULL;
}

char* validate_free_block(mb_free_t *p, mb_free_t *first_free, void *heap_start) {
    if (p == NULL) return NULL;

    size_t heap_end = (size_t) heap_start + MEM_POOL_SIZE;
    size_t pointer_end = (size_t) p + p->size;

    if (p->size > MEM_POOL_SIZE) return "list element size exceeds heap";
    if ((size_t) p < (size_t) heap_start || (size_t) p > heap_end || pointer_end > heap_end) return "list element outside of the heap";
    if (p->next_block != NULL && (size_t) p->next_block <= (size_t) p) return "list element pointing backwards";
    return NULL;
}
