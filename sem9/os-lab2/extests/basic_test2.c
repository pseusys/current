#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is a basic allocator test.\n");
    printf("We are going to try to allocate a large memory block and fail.\n");
    printf("\nNow we will try to allocate a block of size 4100 (it won't fit into the heap space, that is expected to be 4096):\n");
    void *block1 = malloc(4100);
    return 0;
}
