#include "_math.h"

// 2 * size
void add_vectors1 (vector X, vector Y, vector Z, const size_t size)
{
    register unsigned int i ;

#pragma omp parallel for schedule(static)  
    for (i=0; i < size; i++)
        X[i] = Y[i] + Z[i];
  
    return ;
}

// 2 * size
void add_vectors2 (vector X, vector Y, vector Z, const size_t size)
{
    register unsigned int i ;

#pragma omp parallel for schedule(dynamic)  
    for (i=0; i < size; i++)
        X[i] = Y[i] + Z[i];
  
    return ;
}

// 2 * size + 1
double dot1 (vector X, vector Y, const size_t size)
{
    register unsigned int i ;
    register double dot ;

  
    dot = 0.0 ;
#pragma omp parallel for schedule(static) reduction (+:dot)
    for (i=0; i < size; i++)
        dot += X [i] * Y [i];

    return dot ;
}

// 2 * size + 1
double dot2 (vector X, vector Y, const size_t size)
{
    register unsigned int i ;
    register double dot ;


    dot = 0.0 ;
#pragma omp parallel for schedule(dynamic) reduction (+:dot)
    for (i=0; i < size; i++)
        dot += X [i] * Y [i];

    return dot ;
}

// 2 * size + 1
double dot3 (vector X, vector Y, const size_t size)
{
    register unsigned int i ;
    register double dot ;

    dot = 0.0 ;
#pragma omp parallel for schedule(static) reduction (+:dot)
    for (i = 0; i < size; i = i + 8)
    {
        dot += X [i] * Y [i];
        dot += X [i + 1] * Y [i + 1];
        dot += X [i + 2] * Y [i + 2];
        dot += X [i + 3] * Y [i + 3];
    
        dot += X [i + 4] * Y [i + 4];
        dot += X [i + 5] * Y [i + 5];
        dot += X [i + 6] * Y [i + 6];
        dot += X [i + 7] * Y [i + 7];
    }

    return dot ;
}

void mult_mat_vect0 (matrix M, vector b, double *c, size_t datasize)
{
    /*
      matrix vector multiplication
      Sequential function
    */

    return ;
}

void mult_mat_vect1 (matrix M, vector b, vector c, size_t datasize)
{
    /*
      matrix vector multiplication
      Parallel function with static loop scheduling
    */

    return ;
}

void mult_mat_vect2 (matrix M, vector b, vector c, size_t datasize)
{
    /*
      matrix vector multiplication
      Parallel function with static loop scheduling
      unrolling internal loop
    */

    return ;
}

void mult_mat_mat0 (matrix A, matrix B, matrix C, size_t datasize)
{
    /*
      Matrix Matrix Multiplication
      Sequential function 
    */

    return ;    
}


void mult_mat_mat1 (matrix A, matrix B, matrix C, size_t datasize)
{
    /*
      Matrix Matrix Multiplication
      Parallel function with OpenMP and static scheduling 
    */

    return ;    
}

void mult_mat_mat2 (matrix A, matrix B, matrix C, size_t datasize)
{
    /*
      Matrix Matrix Multiplication
      Parallel function with OpenMP and static scheduling 
      Unrolling the inner loop
    */

    return ;
}
