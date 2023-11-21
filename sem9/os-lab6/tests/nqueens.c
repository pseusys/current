/*
 * The program has been modified by T. Ropars
 * Modifications done as part of a lab at IM2AG
 *
 */

/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

/*
 * Original code from the Cilk project (by Keith Randall)
 * 
 * Copyright (c) 2000 Massachusetts Institute of Technology
 * Copyright (c) 2000 Matteo Frigo
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "../tasks.h"
#include "../debug.h"


/* Checking information */

static int solutions[] = {
        1,
        0,
        0,
        2,
        10, /* 5 */
        4,
        40,
        92,
        352,
        724, /* 10 */
        2680,
        14200,
        73712,
        365596,
};

#define MAX_SOLUTIONS sizeof(solutions)/sizeof(int)


/*
 * <a> contains array of <n> queen positions.  Returns 1
 * if none of the queens conflict, and returns 0 otherwise.
 */
static int ok(int n, char *a)
{   
    int i;
    char p;

    if(n == 0){
        return 1;
    }    
    
    char new_pos = a[n-1];

    for (i = 0; i < n-1; i++) {
        p = a[i];
         
        if (new_pos == p || new_pos == p - (n - 1 - i) || new_pos == p + (n - 1 - i))
            return 0;
    }

    return 1;
}

static void print_solution(int n, char *a)
{
    PRINT_DEBUG(1000, "*** Solution: \n");
    
    for(int i=0; i<n; i++){
        PRINT_DEBUG(1000, "%d: %d\n", i, (int)a[i]);
    }

    PRINT_DEBUG(1000, "******\n");
}


task_return_value_t nqueens(task_t *t, unsigned int step)
{
    switch(step){
    case 1:;
        
        int n = *((int*) retrieve_input(t));
        int j = *((int*) retrieve_input(t));
        char *a= (char*) retrieve_input(t);
        
	int i;

        PRINT_DEBUG(1000,"starting executing task for n = %d and j = %d\n",n, j);
        
	if (n == j) {
            int *o = (int*) retrieve_output(t);
            if(ok(j, a)){
                /* good solution, count it */
                *o = 1;
                PRINT_DEBUG(1000, "found a good solution !!\n");
                print_solution(n, a);
            }
            else{
                *o = 0;
            }
            
            return TASK_COMPLETED;
	}
        

        if( ok(j, a)){

            /* try each possible position for queen <j> */
            for (i = 0; i < n; i++) {
                task_t *child = create_task(nqueens);

                int *n1= attach_input(child, sizeof(int));
                *n1 = n;
                int *j1= attach_input(child, sizeof(int));
                *j1 = j+1;
            
                char *b = attach_input(child, n*sizeof(char));
                memcpy(b, a, j*sizeof(char));
                b[j] = (char) i;
            
                int *count_sol = attach_output(child, sizeof(int));
                *count_sol=0;
            
                submit_task(child);
            }
            /* The task will need to know the pb size duing the
             * second part of its execution */
            int *n3= attach_input(t, sizeof(int));
            *n3 = n;

        }
        else {
            int *o = (int*) retrieve_output(t);
            *o = 0;
            PRINT_DEBUG(1000, "Bad solution, cutting the tree\n");
            return TASK_COMPLETED;
        }
        break;

    case 2:; /* after all child tasks are terminated */
        int n2 = *((int*) retrieve_input(t));
        int *o = (int*) retrieve_output(t);
        *o = 0;
	for ( i = 0; i < n2; i++){
            int* val = retrieve_output_from_dependencies(t);
            *o += *val;
        }

        return TASK_COMPLETED;
        
    default:
        PRINT_TEST_FAILED("Error: No step %d for this task\n", step);
        assert(0);
    }

    return TASK_TO_BE_RESUMED;
}

int find_queens (int size)
{
    int res = 0;

    printf("Computing N-Queens algorithm (n=%d) \n", size);
    
    task_t *t = create_task(nqueens);

    int *n= attach_input(t, sizeof(int));
    *n = size;
    int *j= attach_input(t, sizeof(int));
    *j = 0;

    char *a = attach_input(t, size*sizeof(char));
    memset(a, 0, size);
    
    int *out= attach_output(t, sizeof(int));
    *out = 0;
    submit_task(t);

    task_waitall();

    res = *out;
    
    printf(" completed!\n");
    
    return res;
}


void verify_queens (int size, int res)
{
    if ( (unsigned int) size > MAX_SOLUTIONS ){
        printf("Impossible to verify result for size %d\n", size);
        return;
    }
    
    if ( res == solutions[size-1]){
        PRINT_TEST_SUCCESS("Correct result for %d_queens: %d solutions\n", size, res);
    }
    else{
        PRINT_TEST_FAILED("Wrong result for %d_queens: %d solutions instead of %d\n", size, res, solutions[size-1]);
    }
    
}


int main(int argc, char* argv[])
{
    struct timespec begin, end;

    if(argc != 2){
        fprintf(stderr,"Usage: %s pb_size\n", argv[0]);
        exit(-1);
    }

    int pb_size = atoi(argv[1]);

    if(pb_size < 0){
        fprintf(stderr,"Usage: %s pb_size\n", argv[0]);
        exit(-1);
    }
    
    int total_count = 0;
    
    runtime_init_with_deps();
    
    clock_gettime(CLOCK_MONOTONIC, &begin);
    total_count = find_queens(pb_size);
    clock_gettime(CLOCK_MONOTONIC, &end);

    verify_queens(pb_size, total_count);

    runtime_finalize();

    double seconds = end.tv_sec - begin.tv_sec;
    double nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;    
    printf("Time measured: %.3lf seconds.\n", elapsed);

    
    return 0;
}


