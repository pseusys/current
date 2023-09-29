#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gaussian.h"
#include "average.h"
#include "mean.h"
#include "histogram.h"
#include "imagine.h"

#include "../../viscomp-lab1/Util.h"


void print_usage(char* app_name) {
    printf("\nUsage: %s FILE_IN FILE_OUT EXECUTION_MODE ...ARGS\n", app_name);
    printf("\tFILE_IN: input image file name\n\tFILE_OUT: output file name\n");
    printf("\tEXECUTION_MODE: one of 'filter' or 'histogram'\n");
    printf("\nIf EXECUTION_MODE is 'filter':\n");
    printf("\t%s FILE_IN FILE_OUT filter FILTER_TYPE TIMES DIMENSION\n", app_name);
    printf("\t\tFILTER_TYPE: type of filter, one of 'gaussian', 'average', 'mean'\n");
    printf("\t\tTIMES: number of times to apply the filter\n");
    printf("\t\tDIMENSION: dimension of filter, odd number\n");
    printf("\nIf EXECUTION_MODE is 'histogram':\n");
    printf("\t...\n\n");
    exit(EXIT_SUCCESS);
}

void filter(char* filter_type, int times, int dimension, char* input_file, char* output_file) {
    if (dimension <= 0 || dimension % 2 == 0 || times <= 0) pm_erreur("Filter dimension should be a positive odd number, times should be a positive number");

    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);

    if (strcmp(filter_type, "gaussian") == 0) filter_all_gaussian(width, height, times, dimension, bytemap);
    else if (strcmp(filter_type, "average") == 0) filter_all_average(width, height, times, dimension, bytemap);
    else if (strcmp(filter_type, "mean") == 0) filter_all_mean(width, height, times, dimension, bytemap);
    else pm_erreur("Unknown filtering type");

    write_pgm(output_file, width, height, maxval, bytemap);
}

void histogram(char* input_file, char* output_file) {
    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);

    long long int* container = calculate_histogram(width, height, maxval, bytemap);
    write_histogram(output_file, maxval, container);
    free(bytemap);
}

int main(int argc, char* argv[]) {
    if (argc < 4) print_usage(argv[0]);

    if (strcmp(argv[3], "filter") == 0) {
        if (argc != 7) pm_erreur("Wrong argument number for filtering");
        else filter(argv[4], atoi(argv[5]), atoi(argv[6]), argv[1], argv[2]);
    } else if (strcmp(argv[3], "histogram") == 0) {
        if (argc != 4) pm_erreur("Wrong argument number for histogram drawing");
        else histogram(argv[1], argv[2]);
    } else pm_erreur("Wrong execution mode");

    return EXIT_SUCCESS;
}
