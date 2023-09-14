#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int compete(int node, int idesc[static 2], int odesc[static 2]) {
    if (fork() == 0) {
        close(idesc[1]);
        close(odesc[0]);

        int pid = getpid();
        srand(pid);
        int *income, *payload = { pid, rand() };
        read(idesc[0], &income, 2 * sizeof(int));
        if (income[1] > payload[1]) payload = income;
        printf("Process pid %d node %d val = %d\n", pid, node, payload[1]);
        write(odesc[1], payload, 2 * sizeof(int));

        close(idesc[0]);
        close(odesc[1]);
        return EXIT_SUCCESS;
    }
}


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong argument count: %d\n", argc);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    int process_num = atoi(argv[1]);
    int head_descriptors[4];
    pipe(head_descriptors);
    pipe(head_descriptors + 2);
    int *income, *payload = { getpid(), rand() };
    printf("Head process pid %d val = %d\n", payload[0], payload[1]);
    write(head_descriptors[3], payload, 2 * sizeof(int));

    int child_descriptors[4 * process_num];
    for (int i = 1; i < process_num; i++) {
        if (i == 1) {
            pipe(child_descriptors);
            compete(i, head_descriptors + 2, child_descriptors);
        }
        if (i == process_num - 1) {
            pipe(child_descriptors + 2);
            compete(i, child_descriptors + 2, head_descriptors);
        }
        if (i != 1 && i != process_num - 1) {
            pipe(child_descriptors);
            pipe(child_descriptors + 2);
            compete(i, child_descriptors, child_descriptors + 2);
        }
    }

    read(head_descriptors[0], &income, 2 * sizeof(int));
    if (income[1] > payload[1]) payload = income;
    printf("Winner process pid %d val = %d\n", payload[0], payload[1]);

    for (int i = 0; i < process_num; i++) wait(NULL);
    return EXIT_SUCCESS;
}
