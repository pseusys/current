#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sorting.h"

double experiments[NBEXPERIMENTS];

double average_time (void)
{
    unsigned int i ;
    double s = 0 ;

    for (i = 0; i < NBEXPERIMENTS; i++)
    {
        s = s + experiments [i] ;
    }

    return s / NBEXPERIMENTS ;
}


void init_array_sequence (uint64_t *T, uint64_t size)
{
    uint64_t i;

    for (i = 0 ; i < size ; i++)
    {
        T [i] = size - i ;
    }
}

void init_array_random (uint64_t *T, uint64_t size)
{
    uint64_t i;

    srand(time(NULL));

    for (i = 0 ; i < size ; i++)
    {
        T [i] = rand() % size;
    }
}



void print_array (uint64_t *T, uint64_t size)
{
    uint64_t i ;

    for (i = 0 ; i < size ; i++)
    {
        printf ("%ld ", T[i]) ;
    }
    printf ("\n") ;
}

/* test if the array is sorted assuming that the elements to be sorted
 * is a sequence of consecutive values */
int is_sorted_sequence (uint64_t *T, uint64_t size)
{
    uint64_t i ;
  
    for (i = 1 ; i < size ; i++)
    {
        /* test designed specifically for our usecase */
        if (T[i-1] +1  != T [i] )
            return 0 ;
    }
    return 1 ;
}

/* test if the array is sorted */
int is_sorted (uint64_t *T, uint64_t size)
{
    uint64_t i ;
  
    for (i = 1 ; i < size ; i++)
    {
        if (T[i-1] > T [i] ){
            return 0 ;
        }
    }
    return 1 ;
}

/* test if T1 and T2 are equal */
int are_vector_equals (uint64_t *T1, uint64_t *T2, uint64_t size)
{
    uint64_t i ;
  
    for (i = 0 ; i < size ; i++)
    {
        if (T1[i] != T2 [i] ){
            return 0 ;
        }
    }
    return 1 ;
    
}


/* 
   Merge two sorted chunks of array T!
   The two chunks are of size size
   First chunck starts at T[0], second chunck starts at T[size]
*/
void merge(uint64_t *T, const uint64_t size) {
  uint64_t *X = (uint64_t *) malloc (2 * size * sizeof(uint64_t)) ;
  
  uint64_t i = 0;
  uint64_t j = size;
  uint64_t k = 0;

  while ((i < size) && (j < 2*size)) {
    if (T[i] < T[j]) {
      X[k] = T[i];
      i = i + 1;
    } else {
      X[k] = T[j];
      j = j + 1;
    }
    k = k + 1;
  }

  if (i < size) {
    memcpy(X + k, T + i, (size - i) * sizeof(uint64_t));
  } else {
    memcpy(X + k, T + j, (2 * size - j) * sizeof(uint64_t));
  }
  
  memcpy(T, X, 2*size*sizeof(uint64_t));
  free(X);
}
