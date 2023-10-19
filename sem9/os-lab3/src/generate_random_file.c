#define _GNU_SOURCE 

#include <fcntl.h>


#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>
#include <unistd.h>

#include <string.h>


int main(int argc, char **argv)
{
    int fd;
    int *array;

    int size = 10*1024*1024;
    int final_size=0;
    int i=0;

    char filename[256];

    if (argc != 2){
        fprintf(stderr, "usage : %s nb_MB\n", argv[0]) ;
        exit (-1) ;
    }

    size = atoi (argv[1]) ;
    final_size = size * 1024 *1024;

    memset(filename, 0, 256);
    
    sprintf(filename, "/tmp/random_int_%dMB.data",size);
    
    srand(time(NULL));

    if((fd = open(filename, O_CREAT|O_RDWR, S_IRWXU))==-1){
        perror("file opening");
    }

    if(fallocate(fd, 0, 0, final_size*sizeof(int)) == -1){
        perror("fallocate");
    }

    
    if((array = (int*)mmap(NULL, final_size*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        perror("mmap");
    }
    
    for(i=0; i < final_size; i++){
        array[i] = (double) 10 * rand() / RAND_MAX;
    }

    munmap(array, final_size*sizeof(int));
    close(fd);
    
    return 0;
}
