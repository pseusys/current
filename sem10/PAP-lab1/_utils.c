#include <stdlib.h>
#include <stdio.h>
#include "_utils.h"

long long unsigned int average (long long unsigned int *exps)
{
    unsigned int i ;
    long long unsigned int s = 0 ;

    for (i = 2; i < (NBEXPERIMENTS-2); i++)
    {
        s = s + exps [i] ;
    }

    return s / (NBEXPERIMENTS-2) ;
}


void init_vector (vector *X, const size_t size, const double val)
{
    unsigned int i=0;
    
    *X = malloc(sizeof(double) * size);

    if (*X == NULL){
        perror("vector allocation");
        exit(-1);
    }
    

    for (i = 0 ; i < size; i++)
        (*X)[i] = val ;

    return ;
}

void free_vector (vector X)
{
    free(X);
    
    return ;
}


void init_matrix (matrix *X, const size_t size, const double val)
{
    unsigned int i=0, j=0;
    *X = malloc(sizeof(double) * size * size);

    if (*X == NULL){
        perror("matrix allocation");
        exit(-1);
    }

  
    for (i = 0; i < size; i++)
    {
        for (j = 0 ;j < size; j++)
	{
            (*X)[i*size + j] = val ;
	}
    }
}

void free_matrix (matrix X)
{
    free(X);
    
    return ;
}


  
void print_vectors (vector X, vector Y, const size_t size)
{
    unsigned int i ;

    for (i = 0 ; i < size; i++)
        printf (" X [%d] = %le Y [%d] = %le\n", i, X[i], i,Y [i]) ;

    return ;
}
