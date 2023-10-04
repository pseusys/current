#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is an advanced safety test.\n");
    printf("The second free block corruption case is overwriting block next value or size so that it exceeds heap size (considered to be 4096 bytes):\n");
    printf("\nFirst we will allocate a memory block:\n");
    void *block1 = malloc(128);
    printf("\nNow we will alter the header of the subsequent free block so that it will have size of 32 bytes and the next block will be outside of the heap:\n");
    size_t *free_header = (size_t *) ((size_t) block1 + 128);
    free_header[0] = 16;
    free_header[1] = (size_t) block1 + 4100;
    printf("\nNow we will try to allocate another block of size 32 and see an error:\n");
    void *block2 = malloc(32);
    return 0;
}
