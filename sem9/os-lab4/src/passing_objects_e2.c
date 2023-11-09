#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREADS 8

#define LOOPS 20

#define BUF_SIZE 4

int available = 0;
int queue_head = 0;
int buffer[BUF_SIZE];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void pushing(int val)
{
    pthread_mutex_lock(&mutex);
    while (available >= BUF_SIZE) pthread_cond_wait(&condition, &mutex);

    int current_set = (queue_head + available) % BUF_SIZE;
    printf("pushing value %d at %d\n", val, current_set);
    buffer[current_set] = val;
    available++;
    pthread_cond_signal(&condition);

    pthread_mutex_unlock(&mutex);
}

int fetching(void)
{
    pthread_mutex_lock(&mutex);
    while (available <= 0) pthread_cond_wait(&condition, &mutex);

    int val = buffer[queue_head];
    printf("\tfeched value %d at %d\n", val, queue_head);
    queue_head = (queue_head + 1) % BUF_SIZE;
    available--;
    pthread_cond_signal(&condition);

    pthread_mutex_unlock(&mutex);
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
