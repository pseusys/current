#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is a basic safety test.\n");
    printf("First of all we'll check incorrect free invocations, program should output a warning and continue.\n");
    printf("\nFirst we will allocate a memory block:\n");
    void *block1 = malloc(128);
    printf("\nNow we will create a fragmented memory leak and see the warning (we will have 2 blocks of size 128 + two block headers of size 8):\n");
    void *block2 = malloc(5);
    void *block3 = malloc(128);
    free(block2);
    return 0;
}
