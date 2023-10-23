#ifndef VISCOMP_LAB_4_SEGMENTATION_H
#define VISCOMP_LAB_4_SEGMENTATION_H

#include <stdio.h>

#include "../../viscomp-lab1/Util.h"

extern int KMEAN_MAX_ITERATIONS;

void intencity_segmentation(int clusters, int maxval, int length, byte* source);
void intencity_location_segmentation(int clusters, int maxval, int width, int height, byte* source);
void rgb_segmentation(int clusters, int maxval, int length, byte* source);
void rgb_location_segmentation(int clusters, int maxval, int width, int height, byte* source);

#endif // VISCOMP_LAB_4_SEGMENTATION_H
