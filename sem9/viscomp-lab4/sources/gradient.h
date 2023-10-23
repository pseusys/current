#ifndef VISCOMP_LAB_3_GRADIENT_H
#define VISCOMP_LAB_3_GRADIENT_H

#include "../../viscomp-lab1/Util.h"

typedef enum {
  DEG_0 = 0,
  DEG_45 = 1,
  DEG_90 = 2,
  DEG_135 = 3,
  DEG_180 = 4,
  DEG_225 = 5,
  DEG_270 = 6,
  DEG_315 = 7
} Angle;

typedef struct {
    byte magnitude;
    Angle angle;
} Gradient;

void (*get_gradient_one(char* gradient_type)) (int, int, int, byte*, Gradient*);
void gradient_all(char* gradient_type, char* padding_type, int maxval, int width, int height, byte* source);

#endif // VISCOMP_LAB_3_GRADIENT_H
