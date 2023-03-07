#include <stdio.h>
#include <math.h>
#include <omp.h>


#include "_utils.h"
#include "_math.h"
#include <x86intrin.h>

/* default size */
#define DEF_SIZE    1024

/* Processor frequency in GHZ */
#define PROC_FREQ   2.4

#define CHUNK       4

typedef double *vector;

typedef double *matrix;


static long long unsigned int experiments [NBEXPERIMENTS] ;


void init_matrix_inf (matrix* X, const size_t size, double val)
{
    unsigned int i=0, j=0;
    *X = malloc(sizeof(double) * size * size);
    
    if (*X == NULL){
        perror("matrix allocation");
        exit(-1);
    }

  
    for (i = 0; i < size ; i++)
    {
        for (j = 0 ; j < i; j++)
	{
            (*X) [i * size + j] = val ;
            (*X) [j * size + i] = 0.0 ;
	}
        (*X) [i * size + i] = val ;
    }
}


void print_matrix (matrix M, const size_t size)
{
    unsigned int i, j ;

    for (i = 0 ; i < size; i++)
    {
        for (j = 0 ; j < size; j++)
	{
            printf (" %3.2f ", M[i * size + j]) ;
	}
        printf ("\n") ;
    }
    printf ("\n") ;
    return ;
}



void mult_mat_vector (matrix M, vector b, vector c, const size_t size)
{
    register unsigned int i ;
    register unsigned int j ;
    register double r ;
  
    for ( i = 0 ; i < size ; i = i + 1)
    {
        r = 0.0 ;
        for (j = 0 ; j < size ; j = j + 1)
	    {
            r += M [i * size + j] * b [j] ;
	    }
        c [i] = r ;
    }
    return ;
}

void mult_mat_vector_tri_inf (matrix M, vector b, vector c, const size_t size)
{
    register unsigned int i ;
    register unsigned int j ;
    register double r ;

    for ( i = 0 ; i < size ; i = i + 1)
    {
        r = 0.0 ;
        for (j = 0 ; j <= i ; j = j + 1)
	    {
            r += M [i * size + j] * b [j] ;
	    }
        c [i] = r ;
    }
    return ;
}

void mult_mat_vector_tri_inf1 (matrix M, vector b, vector c, const size_t size)
{ 
    register unsigned int i ;
    register unsigned int j ;
    register double r ;

    #pragma omp parallel shared(M, b, c, size) private(i, j, r) 
    {
        #pragma omp for schedule(static)
        for ( i = 0 ; i < size ; i = i + 1)
        {
            r = 0.0 ;
            for (j = 0 ; j <= i ; j = j + 1)
            {
                r += M [i * size + j] * b [j] ;
            }
            c [i] = r ;
        }
    }
    return ;

}

void mult_mat_vector_tri_inf2 (matrix M, vector b, vector c, const size_t size)
{
    register unsigned int i ;
    register unsigned int j ;
    register double r ;

   #pragma omp parallel shared(M, b, c, size) private(i, j, r)
    {
        #pragma omp for schedule(dynamic)
        for ( i = 0 ; i < size ; i = i + 1)
        {
            r = 0.0 ;
            for (j = 0 ; j <= i ; j = j + 1)
            {
                r += M [i * size + j] * b [j] ;
            }
            c [i] = r ;
        }
    }
    return ;
}

void mult_mat_vector_tri_inf3 (matrix M, vector b, vector c, const size_t size)
{
    register unsigned int i ;
    register unsigned int j ;
    register double r ;

    #pragma omp parallel shared(M, b, c, size) private(i, j, r)
    {
        #pragma omp for schedule(guided)
        for ( i = 0 ; i < size ; i = i + 1)
        {
            r = 0.0 ;
            for (j = 0 ; j <= i ; j = j + 1)
            {
                r += M [i * size + j] * b [j] ;
            }
            c [i] = r ;
        }
    }
    return ;
}

void mult_mat_vector_tri_inf4 (matrix M, vector b, vector c, const size_t size)
{
    register unsigned int i ;
    register unsigned int j ;
    register double r ;

    #pragma omp parallel shared(M, b, c, size) private(i, j, r)
    {
        #pragma omp for schedule(runtime)
        for ( i = 0 ; i < size ; i = i + 1)
        {
            r = 0.0 ;
            for (j = 0 ; j <= i ; j = j + 1)
            {
                r += M [i * size + j] * b [j] ;
            }
        c [i] = r ;
        }
    }
    return ;
    
}


int main (int argc, char *argv[])  
{
    matrix M ;
    vector v1 ;
    vector v2 ;

    size_t datasize = DEF_SIZE;

    unsigned long long start, end ;
    unsigned long long residu ;

    unsigned long long av ;
    unsigned int exp ;

    if(argc > 2){
        printf("usage: %s [data_size]\n", argv[0]);
        exit(-1);
    }
    else{
        if(argc == 2){
            datasize = atoi(argv[1]);
        }
    }

    printf("Testing with Matrices of size %zu X %zu\n\n", datasize, datasize);

    printf ("number of threads %d\n", omp_get_max_threads ()) ;
  
    /* rdtsc: read the cycle counter */
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;
  
    init_vector (&v1, datasize, 1.0) ;
    init_matrix_inf (&M, datasize, 2.0) ;
    init_vector (&v2, datasize, 0.0) ;
  
    /* print_vector (v1, datasize) ; */
    /* print_matrix (M, datasize) ; */
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vector (M, v1, v2, datasize)   ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Full matrix multiplication vector \t\t  %Ld cycles\n", av-residu) ;

    /* rdtsc: read the cycle counter */
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;

    free_vector(v1);
    free_vector(v2);
    free_matrix(M);

    init_vector (&v1, datasize, 1.0) ;
    init_matrix_inf (&M, datasize, 2.0) ;
    init_vector (&v2, datasize, 0.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vector_tri_inf (M, v1, v2, datasize)  ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Triangular Matrix multiplication vector\t\t  %Ld cycles\n", av-residu) ;

    /* rdtsc: read the cycle counter */
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;

    free_vector(v1);
    free_vector(v2);
    free_matrix(M);

    init_vector (&v1, datasize, 1.0) ;
    init_matrix_inf (&M, datasize, 2.0) ;
    init_vector (&v2, datasize, 0.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vector_tri_inf1 (M, v1, v2, datasize)  ;

        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;  
  
    printf ("Parallel Loop Static Scheduling \t\t  %Ld cycles\n", av-residu) ;

    /* rdtsc: read the cycle counter */
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;

    free_vector(v1);
    free_vector(v2);
    free_matrix(M);

    init_vector (&v1, datasize, 1.0) ;
    init_matrix_inf (&M, datasize, 2.0) ;
    init_vector (&v2, datasize, 0.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vector_tri_inf2 (M, v1, v2, datasize)  ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Parallel Loop Dynamic Scheduling \t\t  %Ld cycles\n", av-residu) ;

    /* rdtsc: read the cycle counter */
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;

    free_vector(v1);
    free_vector(v2);
    free_matrix(M);

    init_vector (&v1, datasize, 1.0) ;
    init_matrix_inf (&M, datasize, 2.0) ;
    init_vector (&v2, datasize, 0.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vector_tri_inf3 (M, v1, v2, datasize)  ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Parallel Loop Guided Scheduling \t\t  %Ld cycles\n", av-residu) ;

    /* rdtsc: read the cycle counter */
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;

    free_vector(v1);
    free_vector(v2);
    free_matrix(M);

    init_vector (&v1, datasize, 1.0) ;
    init_matrix_inf (&M, datasize, 2.0) ;
    init_vector (&v2, datasize, 0.0) ;

  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vector_tri_inf4 (M, v1, v2, datasize)  ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Parallel Loop Runtime Scheduling \t\t  %Ld cycles\n", av-residu) ;

    return 0;
  
}

