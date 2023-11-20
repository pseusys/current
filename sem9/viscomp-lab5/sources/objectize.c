#include <stdlib.h>
#include <stdio.h>

#include "objectize.h"
#include "imagine.h"


void multiply_matrixes(float *A, float *B, float *result, int rowsA, int colsA, int rowsB, int colsB) {
    if (colsA != rowsB) {
        fprintf(stderr, "Cannot perform matrix multiplication. Invalid dimensions.\n");
        exit(1);
    }

    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            result[i * colsB + j] = 0;
            for (int k = 0; k < colsA; k++) {
                result[i * colsB + j] += A[i * colsA + k] * B[k * colsB + j];
            }
        }
    }
}

byte* dump_points(struct point3d* points, int number, int dimension_x, int dimension_y) {
    byte* bytemap = malloc(dimension_x * dimension_y * RGB_TRIPLET * sizeof(byte));
    for (int i = 0; i < number; i++) {
        if (points[i].x < 0 || points[i].x >= dimension_x || points[i].y < 0 || points[i].y >= dimension_y) continue;
        bytemap[(((int) points[i].x) * dimension_y + ((int) points[i].y)) * RGB_TRIPLET] = points[i].r;
        bytemap[(((int) points[i].x) * dimension_y + ((int) points[i].y)) * RGB_TRIPLET + 1] = points[i].g;
        bytemap[(((int) points[i].x) * dimension_y + ((int) points[i].y)) * RGB_TRIPLET + 2] = points[i].b;
    }
    return bytemap;
}

void rigid_transform(struct point3d* points, int number, float roll, float pitch, float yaw, float T_x, float T_y, float T_z) {
    float trans_matrix[16];
    computeTrans(roll, pitch, yaw, T_x, T_y, T_z, trans_matrix);

    float point[4], result[4];
    for (int i = 0; i < number; i++) {
        point[0] = points[i].x;
        point[1] = points[i].y;
        point[2] = points[i].z;
        point[3] = 1.0;

        multiply_matrixes(trans_matrix, point, result, 4, 4, 4, 1);

        points[i].x = result[0];
        points[i].y = result[1];
        points[i].z = result[2];
    }
}

void pinhole_projection(struct point3d* points, int number, float f) {
    for (int i = 0; i < number; i++) {
        float factor = 1.0 + points[i].z / f;
        points[i].x = points[i].x / factor;
        points[i].y = points[i].y / factor;
    }
}

void u_v_projection(struct point3d* points, int number, float u, float v, float a_u, float a_v) {
    for (int i = 0; i < number; i++) {
        points[i].x = points[i].x / a_u + u;
        points[i].y = points[i].y / a_v + v;
    }
}
