#ifndef __UTILS_H__
#define __UTILS_H__

/* To be called once before starting generating random numbers */
void rand_generator_init(void);

/* Generates a random integer value in range [min, max[ */
int rand_generator_integer(int min, int max);

#endif
