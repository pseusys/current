#ifndef VISCOMP_LAB_2_HISTOGRAM_H
#define VISCOMP_LAB_2_HISTOGRAM_H

#include <stdio.h>

#include "../../viscomp-lab1/Util.h"

/**
 * GENERATES: return value
*/
long long int* calculate_histogram(int width, int height, int maxval, byte* source);

/**
 * CONSUMES: container
*/
void write_histogram(char* output, int size, long long int* container);

#endif // VISCOMP_LAB_2_HISTOGRAM_H
