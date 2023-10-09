#ifndef VISCOMP_LAB_2_GRADIENT_H
#define VISCOMP_LAB_2_GRADIENT_H

#include "../../viscomp-lab1/Util.h"

void scharr_gradient_all(char* padding_type, int maxval, int width, int height, byte* source);
void sobel_gradient_all(char* padding_type, int maxval, int width, int height, byte* source);
void prewitt_gradient_all(char* padding_type, int maxval, int width, int height, byte* source);
void roberts_gradient_all(char* padding_type, int maxval, int width, int height, byte* source);

#endif // VISCOMP_LAB_2_GRADIENT_H
