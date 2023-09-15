#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong argument count: %d\n", argc);
        return EXIT_FAILURE;
    }
    int process_num = atoi(argv[1]);

    for (int i = 0; i < process_num; i++) if (fork() == 0) {
        sleep(i);
        printf("Current process id: %d\n", getpid());
        return EXIT_SUCCESS;
    }

    for (int i = 0; i < process_num; i++) {
        int process_status, process_id = wait(&process_status);
        printf("Child %d finished with status %d\n", process_id, process_status);
    }
    return EXIT_SUCCESS;
}
