#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

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

void object(char* input_file, char* output_file) {
    struct point3d* points;
    int N_v = 0;
    points = readOff(input_file, &N_v);
    centerThePCL(points, N_v);

    int dimension = 1024;
    transform(points, N_v, 0.0, 0.0, 0.0, 0, 0.0, 100.0);
    byte* bytemap = dump_points(points, N_v);

    write_ppm(output_file, dimension, dimension, 255, bytemap);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (argc < 4) print_usage(argv[0]);

    if (strcmp(argv[3], "object") == 0) {
        if (argc != 4) pm_erreur("Wrong argument number for objectivisation");
        else object(argv[1], argv[2]);
    } else pm_erreur("Wrong execution mode");

    return EXIT_SUCCESS;
}
