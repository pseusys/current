#include <stdio.h>
#include <string.h>

#include "mem_alloc_sim.h"

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
    size_t pool_size=1;
    policy_t pool_policy = BF;
    int align = 1;
    
    if(argc > 1){
        pool_size = atoi(argv[1]);
        printf("Sim_Config: pool size -> %zu\n", pool_size);

        if(argc > 2){
            if(!strcmp(argv[2], "WF")){
                pool_policy = WF;
            }
            else if(!strcmp(argv[2], "FF")){
                pool_policy = FF;
            }
            else if(!strcmp(argv[2], "NF")){
                pool_policy = NF;
            }
        }
        if(argc > 3){
            int val = atoi(argv[3]);
            align = (val <=64 && val > 1)? val : 1;
        }
    }
    else{
        printf("Config: running test with the default configuration\n");
    }
        
    memset(block_pointer,0,1024*sizeof(char*));
    
    memory_init_sim(pool_size, pool_policy, align);

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
            break;
        case 'h':
            help();
            break;
        case 'q':
            exit(0);
        default:
            fprintf(stderr,"Command not found !\n");
        }
        /* empty stdin if some chars remain*/
        fgets(buffer,SIZE_BUFFER,stdin);
    }
    return 0;
}
