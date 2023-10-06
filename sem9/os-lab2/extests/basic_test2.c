#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is a basic allocator test.\n");
    printf("We are going to allocate a block of size 0 th prove it is possible.\n");
    printf("Next we are going to try to allocate a large memory block and fail.\n");
    printf("\nNow we are allocating a zero-size block:\n");
    void *block1 = malloc(0);
    printf("\nNow we will try to allocate a block of size 4100 (it won't fit into the heap space, that is expected to be 4096):\n");
    void *block2 = malloc(4100);
    return 0;
}
