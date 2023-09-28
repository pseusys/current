#ifndef   	_MEM_ALLOC_TYPES_H_
#define   	_MEM_ALLOC_TYPES_H_



/* Structure declaration for a free block */
struct mb_free{
    size_t size;
    struct mb_free *next_block;
}; 
typedef struct mb_free mb_free_t; 

/* Specific metadata for allocated blocks */
struct mb_allocated{
    size_t size;
};
typedef struct mb_allocated mb_allocated_t;


#endif
