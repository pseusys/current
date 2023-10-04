#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;


int main() {
    setbuf(stdout, NULL);
    printf("This program is a basic safety test.\n");
    printf("First of all we'll check incorrect free invocations, program should output a warning and continue.\n");
    printf("\nFirst we will allocate a memory block:\n");
    void *block = malloc(128);
    printf("\nNow we will try to free space before the heap head:\n");
    free((void *) ((size_t) block) - 64);
    printf("\nNow we will try to free space after the heap end (we expect it to be 4096 bytes long):\n");
    free((void *) (((size_t) block) + 4096));
    printf("\nNow we will try to free space inside of the free space:\n");
    free((void *) (((size_t) block) + 256));
    printf("\nAnd now we will mock an allocated block header inside of our block, so that it overlaps the free space and try to free it:\n");
    size_t *pseudo_block = (size_t *) ((size_t) block + sizeof(size_t));
    pseudo_block[-1] = 256;
    free(pseudo_block);
    printf("\nNow we will see memory simple leak warning (we have allocated block of size 128 + header of size 8 and never freed it):\n");
    return 0;
}
