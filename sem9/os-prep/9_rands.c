#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong argument count: %d\n", argc);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    printf("Head process pid %d val = %d\n", getpid(), rand());
    int process_num = atoi(argv[1]);

    for (int i = 1; i < process_num; i++) if (fork() == 0) {
        int pid = getpid();
        srand(pid);
        printf("Process pid %d node %d val = %d\n", pid, i, rand());
        return EXIT_SUCCESS;
    }

    for (int i = 0; i < process_num; i++) wait(NULL);
    return EXIT_SUCCESS;
}
