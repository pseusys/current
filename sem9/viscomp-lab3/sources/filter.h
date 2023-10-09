#ifndef VISCOMP_LAB_2_FILTER_H
#define VISCOMP_LAB_2_FILTER_H

#include "../../viscomp-lab1/Util.h"

void filter_all_average(char* padding_type, int maxval, int width, int height, int ntimes, int dimension, byte* source);
void filter_all_gaussian(char* padding_type, int maxval, int width, int height, int ntimes, int dimension, byte* source);
void filter_all_mean(char* padding_type, int maxval, int width, int height, int ntimes, int dimension, byte* source);

#endif // VISCOMP_LAB_2_FILTER_H
