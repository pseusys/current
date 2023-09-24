#include <stdio.h>
#include <string.h>


#include "mem_alloc.h"

#define SIZE_BUFFER 128

void help(void){
    printf("List of commands:\n");
    printf("\t h -- displays this help message\n");
    printf("\t aXX -- allocates a block of size XX\n");
    printf("\t fY -- frees the block allocated in the Y-th call to malloc\n");
    printf("\t p -- displays the memory state\n");
    printf("\t q -- exists the program\n");
}



int main(int argc, char *argv[]) {
	char buffer[SIZE_BUFFER];
	char command;
        int index;
	int size;

        char* block_pointer[1024];
        int count=1;

        memset(block_pointer,0,1024*sizeof(char*));
        

	memory_init();

	while (1) {
		command = getchar();
		switch (command) {
		case 'a':
                    scanf ("%d",&size);
                    block_pointer[count]=memory_alloc(size);
                    count++;
                    break;
		case 'f': 
                    scanf("%d",&index);
                    memory_free(block_pointer[index]);
                    break;
		case 'p':
                    print_mem_state();
                    break;
		case 'h':
                    help();
                    break;
		case 'q':
                    exit(0);
		default:
                    fprintf(stderr,"Command not found !\n");
		}
		/* empty stdin if some chars remain */
		fgets(buffer,SIZE_BUFFER,stdin);
	}
	return 0;
}
