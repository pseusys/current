#ifndef VISCOMP_LAB_2_CANNYS_H
#define VISCOMP_LAB_2_CANNYS_H

#include "../../viscomp-lab1/Util.h"

void non_max_suppress(char* gradient_type, char* padding_type, int maxval, int width, int height, byte* source);
void hysteresis_threshold(char* gradient_type, char* padding_type, int maxval, int width, int height, byte* source, byte higher, byte lower);

#endif // VISCOMP_LAB_2_CANNYS_H
