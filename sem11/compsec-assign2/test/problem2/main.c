#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

struct PtrBuff {
    char buffer[8];
    char* ptr;
};

void some_func(char* input) {
    int placeholder;
    struct PtrBuff v;
    v.ptr = (char*) 0xdeadbeefdeadbeef;

    strcpy(v.buffer, input);

    printf("Buffer: %s\n", v.buffer);
    printf("Pointer: %p\n", v.ptr);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("problem2: open error");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    if (read(fd, buffer, sizeof(buffer)) < 0) {
        perror("problem2: read failed");
        exit(EXIT_FAILURE);
    }

    some_func(buffer);
    return 0;
}
