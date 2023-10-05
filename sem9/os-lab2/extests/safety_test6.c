#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is an advanced safety test.\n");
    printf("The last free block corruption case is writing in a free block, that can happen after a block was freed for example.\n");
    printf("Upon block freeing, the free space is filled with zeros and if the zeros in a free block are corrupted, that means that a write has taken place.\n");
    printf("\nFirst we will allocate a memory block:\n");
    void *block1 = malloc(128);
    printf("\nNow we will alter a random byte in the heap:\n");
    size_t *random_address = (size_t *) ((size_t) block1 + 256);
    random_address[1] = (size_t) 42;
    printf("\nNow we will try to allocate another block of and see an error:\n");
    void *block2 = malloc(32);
    return 0;
}
