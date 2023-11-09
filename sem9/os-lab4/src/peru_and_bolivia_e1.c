#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define LOOPS 3


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void enter_shared_track() {
    pthread_mutex_lock(&mutex);
}

void exit_shared_track() {
    pthread_mutex_unlock(&mutex);
}

void using_shared_track_section(char* name) {
    int trip_duration = rand() % 6 +1;

    enter_shared_track();
    
    for(; trip_duration>0; trip_duration--){        
        printf("%c ",name[0]);
        fflush(stdout);
        usleep(1);
    }

    printf("\n");
    exit_shared_track();
}


void* train(void* arg) {
    char *country=(char*) arg;
    int i=0;
    
    for(i=0; i<LOOPS; i++){
        
        using_shared_track_section(country);

    }

    return NULL;
}


int main(void) {
    pthread_t tids[2];
    int i=0;
    
    struct timespec tt;
    clock_gettime(CLOCK_MONOTONIC, &tt);
    /* seed for the random number generator */
    srand(tt.tv_sec);
    
    
    /* the peruvian train */
    if(pthread_create (&tids[0], NULL, train, (void*)"PERU") != 0){
        fprintf(stderr,"Failed to create the peruvian thread\n");
        return EXIT_FAILURE;
    }

    /* the bolivian train */
    if(pthread_create (&tids[1], NULL, train, (void*)"BOLIVIA") != 0){
        fprintf(stderr,"Failed to create the peruvian thread\n");
        return EXIT_FAILURE;
    }
    
    /* Wait until every thread ended */ 
    for (i = 0; i < 2; i++){
        pthread_join (tids[i], NULL) ;
    }  
    
    return EXIT_SUCCESS;
}
