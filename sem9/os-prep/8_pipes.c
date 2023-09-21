#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    int descriptors[2];
    pipe(descriptors);
    int process_id = fork();

    if(process_id == 0) {
        int parent_id;
        close(descriptors[1]);
        read(descriptors[0], &parent_id, sizeof(int));
        printf("Parent id: %d\n", parent_id);
        close(descriptors[0]);
    } else {
        int own_id = getpid();
        close(descriptors[0]);
        write(descriptors[1], &own_id, sizeof(int));
        printf("My id: %d\n", own_id);
        close(descriptors[1]);
    }

    return EXIT_SUCCESS;
}
