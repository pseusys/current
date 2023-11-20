#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "imagine.h"
#include "objectize.h"
#include "utils.h"

#include "../../viscomp-lab1/Util.h"


void print_usage(char* app_name) {
    printf("\nUsage: %s FILE_IN FILE_OUT EXECUTION_MODE ...ARGS\n", app_name);
    printf("\tFILE_IN: input image file name\n\tFILE_OUT: output file name\n");
    printf("\tEXECUTION_MODE: one of 'object'\n");
    printf("\nIf EXECUTION_MODE is 'object':\n");
    printf("\t%s FILE_IN FILE_OUT object\n", app_name);
    printf("\n");
    exit(EXIT_SUCCESS);
}

void object(char* input_file, char* output_file, float transform_x, float transform_y, float transform_z, float rotate_x, float rotate_y, float rotate_z, int dimension_x, int dimension_y, float aspect_x, float aspect_y, float center_x, float center_y, float focal) {
    struct point3d* points;
    int N_v = 0;
    points = readOff(input_file, &N_v);
    centerThePCL(points, N_v);

    float rad_rotate_x = rotate_x * M_PI / 180.0;
    float rad_rotate_y = rotate_y * M_PI / 180.0;
    float rad_rotate_z = rotate_z * M_PI / 180.0;

    rigid_transform(points, N_v, transform_x, transform_y, transform_z, rad_rotate_x, rad_rotate_y, rad_rotate_z);
    pinhole_projection(points, N_v, focal);
    u_v_projection(points, N_v, center_x, center_y, aspect_x, aspect_y);

    byte* bytemap = dump_points(points, N_v, dimension_x, dimension_y);
    write_ppm(output_file, dimension_x, dimension_y, 255, bytemap);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (argc < 4) print_usage(argv[0]);

    if (strcmp(argv[3], "object") == 0) {
        if (argc != 17) pm_erreur("Wrong argument number for objectivisation");
        else object(argv[1], argv[2], atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]), atof(argv[8]), atof(argv[9]), atoi(argv[10]), atoi(argv[11]), atof(argv[12]), atof(argv[13]), atof(argv[14]), atof(argv[15]), atof(argv[16]));
    } else pm_erreur("Wrong execution mode");

    return EXIT_SUCCESS;
}
