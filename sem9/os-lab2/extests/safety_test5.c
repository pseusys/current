#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is an advanced safety test.\n");
    printf("The third free block corruption case is having next free block before current (potentially leading to a loop):\n");
    printf("\nFirst we will allocate a memory block:\n");
    void *block1 = malloc(128);
    printf("\nNow we will alter the header of the subsequent free block so that it will point to the block we've just allocated:\n");
    size_t *free_header = (size_t *) ((size_t) block1 + 128);
    free_header[1] = (size_t) block1;
    printf("\nNow we will try to allocate another block of and see an error:\n");
    void *block2 = malloc(32);
    return 0;
}
