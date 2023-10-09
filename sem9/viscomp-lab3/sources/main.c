#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "filter.h"
#include "histogram.h"
#include "imagine.h"
#include "alter.h"
#include "gradient.h"

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

void filter(char* filter_type, char* padding_type, int times, int dimension, char* input_file, char* output_file) {
    if (dimension <= 0 || dimension % 2 == 0 || times <= 0) pm_erreur("Filter dimension should be a positive odd number, times should be a positive number");

    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);

    if (strcmp(filter_type, "gaussian") == 0) filter_all_gaussian(padding_type, maxval, width, height, times, dimension, bytemap);
    else if (strcmp(filter_type, "average") == 0) filter_all_average(padding_type, maxval, width, height, times, dimension, bytemap);
    else if (strcmp(filter_type, "mean") == 0) filter_all_mean(padding_type, maxval, width, height, times, dimension, bytemap);
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

void stretch(char* input_file, char* output_file) {
    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);

    stretch_histogram(width, height, maxval, bytemap);
    write_pgm(output_file, width, height, maxval, bytemap);
}

void equalize(char* input_file, char* output_file) {
    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);

    equalize_histogram(width, height, maxval, bytemap);
    write_pgm(output_file, width, height, maxval, bytemap);
}

void gradient(char* gradient_type, char* padding_type, char* input_file, char* output_file, byte threshold) {
    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);

    if (strcmp(gradient_type, "roberts") == 0) roberts_gradient_all(padding_type, maxval, width, height, bytemap);
    else if (strcmp(gradient_type, "prewitt") == 0) prewitt_gradient_all(padding_type, maxval, width, height, bytemap);
    else if (strcmp(gradient_type, "sobel") == 0) sobel_gradient_all(padding_type, maxval, width, height, bytemap);
    else if (strcmp(gradient_type, "scharr") == 0) scharr_gradient_all(padding_type, maxval, width, height, bytemap);
    else pm_erreur("Unknown gradient type");

    for (int i = 0; i < width * height; i++) bytemap[i] = bytemap[i] > threshold ? bytemap[i] : 0;
    write_pgm(output_file, width, height, maxval, bytemap);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (argc < 4) print_usage(argv[0]);

    if (strcmp(argv[3], "filter") == 0) {
        if (argc != 8) pm_erreur("Wrong argument number for filtering");
        else filter(argv[4], argv[5], atoi(argv[6]), atoi(argv[7]), argv[1], argv[2]);
    } else if (strcmp(argv[3], "histogram") == 0) {
        if (argc != 4) pm_erreur("Wrong argument number for histogram drawing");
        else histogram(argv[1], argv[2]);
    } else if (strcmp(argv[3], "stretch") == 0) {
        if (argc != 4) pm_erreur("Wrong argument number for image stretching");
        else stretch(argv[1], argv[2]);
    } else if (strcmp(argv[3], "equalize") == 0) {
        if (argc != 4) pm_erreur("Wrong argument number for image equalization");
        else equalize(argv[1], argv[2]);
    } else if (strcmp(argv[3], "gradient") == 0) {
        if (argc != 7) pm_erreur("Wrong argument number for image gradient calculation");
        else gradient(argv[4], argv[5], argv[1], argv[2], atoi(argv[6]));
    } else pm_erreur("Wrong execution mode");

    return EXIT_SUCCESS;
}
