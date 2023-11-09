#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NB_THREADS 16

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

int threads_waiting = 0;


void barrier(void)
{
    pthread_mutex_lock(&mutex);
    threads_waiting++;
    if (threads_waiting < NB_THREADS)
        while (threads_waiting != NB_THREADS) pthread_cond_wait(&condition, &mutex);
    else pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);
}



void* thread_routine(void* arg)
{
    unsigned long id = (unsigned long)arg;

    printf("thread %lu at the barrier\n", id);

    barrier();
    
    printf("\tthread %lu after the barrier\n", id);

    return NULL;
}


int main(void)
{
    pthread_t tids[NB_THREADS];
    unsigned long i=0;

    for(i=0; i<NB_THREADS; i++){
        if(pthread_create (&tids[i], NULL, thread_routine, (void*)i) != 0){
            fprintf(stderr,"Failed to create thread %lu\n", i);
            return EXIT_FAILURE;
        }
    }
    
    /* Wait until every thread ended */ 
    for (i = 0; i < NB_THREADS; i++){
        pthread_join (tids[i], NULL) ;
    }  
    
    return EXIT_SUCCESS;
}
