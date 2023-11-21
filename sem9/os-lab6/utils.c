#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "utils.h"

void rand_generator_init(void)
{
    struct timespec tt;
    clock_gettime(CLOCK_MONOTONIC, &tt);

    /* seed for the random number generator */
    srand(tt.tv_sec);
}


int rand_generator_integer(int min, int max)
{
    assert(min < max);
    
    int range = max - min;
    
    int val = rand() % range;

    return val + min;
}
