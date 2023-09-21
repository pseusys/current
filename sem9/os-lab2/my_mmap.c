#include <stdlib.h>
#include <assert.h>

#include "my_mmap.h"
#include "mem_alloc.h"

/* 
 * The address returned by mmap is always a multiple of 4096.
 * We must check that this is compliant with our alignment
 * constraint and, if not, adjust the requested size and
 * the returned address.
 */
static int pad(int alignment) {
    return (4096 % alignment);
}

void *my_mmap(size_t size) {
    void *res;
    int padding;

    padding = pad(MEM_ALIGNMENT);
    if (padding != 0) {
        size += padding;
    }

    res = mmap(NULL,
                size,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANONYMOUS,
                0,
                0);

    if (res != NULL) {
        if (padding != 0) {
            res = (void*)((char*)res + padding);
        }
        assert(((unsigned long)res) % MEM_ALIGNMENT == 0);
    }
    return res;
}

int my_munmap(void *addr, size_t length) {
    int padding;
    void *a = addr;
    size_t l = length;

    padding = pad(MEM_ALIGNMENT);
    if (padding != 0) {
        l += padding;
        a = (void*)((char*)a - padding);
    }   

    return munmap(a, l);
}