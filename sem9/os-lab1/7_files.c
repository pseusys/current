#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


int main() {
    open("training_system/exercise4/file1.txt", O_RDONLY);
    creat("training_system/exercise7/file_copy.txt", S_IWUSR | S_IWGRP | S_IROTH | S_ISUID | S_ISGID);
    return EXIT_SUCCESS;
}
