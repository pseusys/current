#include <stdio.h>
#include <stdlib.h>


int main() {
    setbuf(stdout, NULL);
    printf("This program is an advanced safety test.\n");
    printf("We have already considered allocated block metadata corruption in previous safety tests.\n");
    printf("In general, we were detecting it during 'free' calls only and just ignored the errors as well as if 'free' was called for a wrong memory address.\n");
    printf("Now we will consider three extra cases: three of them will involve free block metadata corruption and the last one will cover use-after-free case.\n");
    printf("We will detect the corruption during both 'alloc' and 'free' calls and every detection will lead to an error.\n");
    printf("The first corruption case is overwriting block size so that it becomes greater than heap size (again, considered to be 4096 bytes):\n");
    printf("\nFirst we will allocate a memory block:\n");
    void *block1 = malloc(128);
    printf("\nNow we will alter the header of the subsequent free block so that it will have size of 4100 bytes:\n");
    size_t *free_header = (size_t *) ((size_t) block1 + 128);
    free_header[0] = 4100;
    printf("\nNow we will try to allocate another block and see an error:\n");
    void *block2 = malloc(16);
    return 0;
}
