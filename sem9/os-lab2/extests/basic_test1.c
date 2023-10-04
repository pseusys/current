#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is a basic allocator test.\n");
    printf("We are going to allocate several memory blocks and then free them.\n");
    printf("\nFirst we will allocate a memory block of size 128 (not that it has a 8 extra byte header):\n");
    void *block1 = malloc(128);
    printf("\nNow we will allocate another two blocks of size 256:\n");
    void *block2 = malloc(256);
    void *block3 = malloc(256);
    printf("\nNow we will free the first block of size 256 and allocate a block of size 128 instead (it will reuse the space):\n");
    free(block2);
    block2 = malloc(128);
    printf("\nNow we will allocate a block of size 512 (it won't fit into the remaining space):\n");
    void *block4 = malloc(512);
    printf("\nNow we will free all the blocks:\n");
    free(block1);
    free(block2);
    free(block3);
    free(block4);
    printf("\nAfter successful program execution the memory layout will be printed:\n");
    return 0;
}
