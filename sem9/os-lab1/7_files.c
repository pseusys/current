#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


int main() {
    int buffer_size = 1000;
    int buffer[buffer_size];

    int source = open("training_system/exercise4/file1.txt", O_RDONLY);
    if (source == -1) printf("Error opening source file!\n");
    int copy = creat("training_system/exercise4/file_copy.txt", S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH | S_IROTH | S_ISUID | S_ISGID);
    if (copy == -1) printf("Error opening copy file!\n");

    int bytes = read(source, buffer, buffer_size);
    while (bytes == buffer_size) {
        write(copy, buffer, buffer_size);
        bytes = read(source, buffer, buffer_size);
    }
    write(copy, buffer, bytes);

    source = close(source);
    if (source == -1) printf("Error closing source file!\n");
    copy = close(copy);
    if (copy == -1) printf("Error closing copy file!\n");

    return EXIT_SUCCESS;
}
