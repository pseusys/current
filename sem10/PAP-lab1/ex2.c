#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include <x86intrin.h>

#include "_utils.h"
#include "_math.h"

/* default size */
#define DEF_SIZE    100

static long long unsigned int experiments [NBEXPERIMENTS] ;


int main (int argc, char *argv[])  
{
    int maxnthreads ;

    vector a, b, c ;
    matrix M1, M2 ;

    size_t datasize = DEF_SIZE;

    unsigned long long int start, end ;
    unsigned long long int measure_cost ;

    unsigned long long int av ;

    int exp ;

    if(argc > 2){
        printf("usage: %s [data_size]\n", argv[0]);
        exit(-1);
    }
    else{
        if(argc == 2){
            datasize = atoi(argv[1]);
        }
    }
  
    printf("Testing with Vectors of size %zu -- Matrices of size %zu X %zu\n\n", datasize, datasize, datasize);
  
    /* 
       rdtsc: read the cycle counter 
    */
  
    start = _rdtsc () ;
    end = _rdtsc () ;
    measure_cost = end - start ;
  
    maxnthreads = omp_get_max_threads () ;
    printf("Max number of threads: %d \n", maxnthreads);
	
    /*
      Vector Initialization
    */

    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&c, datasize, 0.0) ;

    /*
      print_vectors (a, b, datasize) ;
    */    

    printf ("=============== ADD ==========================================\n") ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        add_vectors1 (c, a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP static loop:\t\t %Ld cycles\n", av-measure_cost) ;

    /* code to print the GLOP per seconds */
    /* printf ("%3.3f GFLOP per second\n", [TODO: ADD YOUR COMPUTATION]) ; */

    free_vector(a);
    free_vector(b);
    free_vector(c);
  
    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&c, datasize, 0.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        add_vectors2 (c, a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP dynamic loop:\t\t %Ld cycles\n", av-measure_cost) ;

    free_vector(a);
    free_vector(b);
    free_vector(c);
  
    printf ("==============================================================\n\n") ;

    printf ("==================== DOT =====================================\n") ;

    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        dot1 (a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("dot OpenMP static loop:\t\t\t %Ld cycles\n", av-measure_cost) ;

    free_vector(a);
    free_vector(b);
  
    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        dot2 (a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("dot OpenMP dynamic loop:\t\t %Ld cycles\n", av-measure_cost) ;

    free_vector(a);
    free_vector(b);  
  
    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        dot3 (a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("dot OpenMP static unrolled loop:\t %Ld cycles\n", av-measure_cost) ;

    free_vector(a);
    free_vector(b);
  
    printf ("=============================================================\n\n") ;

    printf ("======================== Mult Mat Vector =====================================\n") ;
  
    init_matrix (&M1, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&a, datasize, 0.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vect0 (M1, b, a, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Sequential matrice vector multiplication:\t %Ld cycles\n", av-measure_cost) ;

    free_matrix(M1);
    free_vector(a);
    free_vector(b);


    init_matrix (&M1, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&a, datasize, 0.0) ;


    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vect1 (M1, b, a, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
 
    printf ("OpenMP static loop MultMatVect1:\t\t %Ld cycles\n", av-measure_cost) ;


    free_matrix(M1);
    free_vector(a);
    free_vector(b);
  
    init_matrix (&M1, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&a, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vect2 (M1, b, a, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP static unrolled loop MultMatVect2:\t\t %Ld cycles\n", av-measure_cost) ;

    free_matrix(M1);
    free_vector(a);
    free_vector(b);
  

    printf ("===================================================================\n\n") ;

    printf ("======================== Mult Mat Mat =====================================\n") ;
  
    init_matrix (&M1, datasize, 1.0) ;
    init_matrix (&M2, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_mat0 (M1, M2, M2, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("Sequential Matrix Matrix Multiplication:\t %Ld cycles\n", av-measure_cost) ;

    free_matrix(M1);
    free_matrix(M2);
  
    init_matrix (&M1, datasize, 1.0) ;
    init_matrix (&M2, datasize, 2.0) ;


    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_mat1 (M1, M2, M2, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP static loop matrix matrix multiplication: %Ld cycles\n", av-measure_cost) ;

    free_matrix(M1);
    free_matrix(M2);

    init_matrix (&M1, datasize, 1.0) ;
    init_matrix (&M2, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_mat2 (M1, M2, M2, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
    

    printf ("OpenMP static unrolled loop matrix matrix multiplication: %Ld cycles\n", av-measure_cost) ;
  
    free_matrix(M1);
    free_matrix(M2);

    printf ("===================================================================\n\n") ;
  
    return 0;
  
}
