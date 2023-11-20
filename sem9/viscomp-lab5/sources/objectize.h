#ifndef VISCOMP_LAB_5_OBJECTIZE_H
#define VISCOMP_LAB_5_OBJECTIZE_H

#include <stdio.h>

#include "utils.h"
#include "../../viscomp-lab1/Util.h"

/**
 * GENERATES: return value
*/
byte* dump_points(struct point3d* points, int number, int dimension_x, int dimension_y);

void rigid_transform(struct point3d* points, int number, float roll, float pitch, float yaw, float T_x, float T_y, float T_z);

void pinhole_projection(struct point3d* points, int number, float f);

void u_v_projection(struct point3d* points, int number, float u, float v, float a_u, float a_v);

#endif // VISCOMP_LAB_5_OBJECTIZE_H
