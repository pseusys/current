#include <stdlib.h>
#include <stdio.h>

#include "objectize.h"
#include "imagine.h"


int dimension = 1024;


byte* dump_points(struct point3d* points, int number) {
    byte* bytemap = malloc(dimension * dimension * RGB_TRIPLET * sizeof(byte));
    for (int i = 0; i < number; i++) {
        printf("%lf %lf %lf\n", points[i].x, points[i].y, points[i].z);
        bytemap[(((int) points[i].x) * dimension + ((int) points[i].y)) * RGB_TRIPLET] = points[i].r;
        bytemap[(((int) points[i].x) * dimension + ((int) points[i].y)) * RGB_TRIPLET + 1] = points[i].g;
        bytemap[(((int) points[i].x) * dimension + ((int) points[i].y)) * RGB_TRIPLET + 2] = points[i].b;
    }
    return bytemap;
}

void transform(struct point3d* points, int number, float roll, float pitch, float yaw, float T_x, float T_y, float T_z) {
    float trans_matrix[16];
    computeTrans(roll, pitch, yaw, T_x, T_y, T_z, trans_matrix);

    float point[4];
    float result[4];
    for (int i = 0; i < number; i++) {
        point[0] = points[i].x;
        point[1] = points[i].y;
        point[2] = points[i].z;
        point[3] = 1.0;

        // printf("%lf %lf %lf %lf\n", point[0], point[1], point[2], point[3]);
        for (int k = 0; k < 4; k++) {
            result[k] = 0.0;
            for (int j = 0; j < 4; j++) {
                result[k] += point[j] * trans_matrix[k * 4 + j];
                // printf("%lf += %lf * %lf (%lf)\n", result[k], point[j], trans_matrix[k * 4 + j], point[j] * trans_matrix[k * 4 + j]);
            }
            // printf("%d: %lf %lf %lf %lf\n", k, trans_matrix[k * 4 + 0], trans_matrix[k * 4 + 1], trans_matrix[k * 4 + 2], trans_matrix[k * 4 + 3]);
            // printf("%d: %lf %lf %lf %lf\n", k, result[0], result[1], result[2], result[3]);
        }
        // printf("%lf %lf %lf %lf\n", result[0], result[1], result[2], result[3]);
        // printf("\n\n");

        points[i].x = result[0];
        points[i].y = result[1];
        points[i].z = result[2];
    }
}
