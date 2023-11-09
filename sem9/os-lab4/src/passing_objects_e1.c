#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREADS 8

#define LOOPS 20

#define BUF_SIZE 4

int current_set = 0;
int current_get = 0;
int buffer[BUF_SIZE];

sem_t mutex_semaphore;
sem_t overflow_semaphore;
sem_t underflow_semaphore;

void pushing(int val)
{
    sem_wait(&overflow_semaphore);
    sem_wait(&mutex_semaphore);

    printf("pushing value %d at %d\n", val, current_set);
    buffer[current_set] = val;
    current_set = (current_set + 1) % BUF_SIZE;

    sem_post(&mutex_semaphore);
    sem_post(&underflow_semaphore);
}

int fetching(void)
{
    int val=0;

    sem_wait(&underflow_semaphore);
    sem_wait(&mutex_semaphore);

    val = buffer[current_get];
    printf("\tfeched value %d at %d\n", val, current_get);
    current_get = (current_get + 1) % BUF_SIZE;

    sem_post(&mutex_semaphore);
    sem_post(&overflow_semaphore);

    return val;
}

void* thread_generating(void* arg)
{
    int i = 0;
    int new_value = 0;
    
    for(i=0; i<LOOPS; i++){
        new_value = rand()%10;
        pushing(new_value);
    }

    return NULL;
}


void* thread_using(void* arg)
{
    int i = 0;
    int value = 0;
    
    for(i=0; i<LOOPS; i++){
        value += fetching();
    }

    printf("sum of values: %d\n",value);
    return NULL;
}



int main(void)
{
    pthread_t tids[THREADS];
    int i=0;

    sem_init(&mutex_semaphore, 0, 1);
    sem_init(&overflow_semaphore, 0, BUF_SIZE);
    sem_init(&underflow_semaphore, 0, 0);

    struct timespec tt;
    clock_gettime(CLOCK_MONOTONIC, &tt);
    /* seed for the random number generator */
    srand(tt.tv_sec);
    
    for (int i = 0; i < THREADS / 2; i++)
        if(pthread_create (&tids[0], NULL, thread_using, NULL) != 0){
            fprintf(stderr,"Failed to create the using thread\n");
            return EXIT_FAILURE;
        }
    
    for (int i = THREADS / 2; i < THREADS; i++)
        if(pthread_create (&tids[1], NULL, thread_generating, NULL) != 0){
            fprintf(stderr,"Failed to create the generating thread\n");
            return EXIT_FAILURE;
        }
    
    /* Wait until every thread ended */ 
    for (i = 0; i < 2; i++){
        pthread_join (tids[i], NULL) ;
    }  
    
    return EXIT_SUCCESS;
}
