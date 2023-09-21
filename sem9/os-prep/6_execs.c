#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    int process_id = fork();
    if (process_id == 0) {
        printf("Current process id: %d\n", execl("/bin/ls", "ls", NULL));
    } else {
        int child_status;
        printf("Hello world!\n");
        wait(&child_status);
    }
    return EXIT_SUCCESS;
}
