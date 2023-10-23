#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "filter.h"
#include "histogram.h"
#include "imagine.h"
#include "alter.h"
#include "gradient.h"
#include "cannys.h"
#include "segmentation.h"

#include "../../viscomp-lab1/Util.h"


void print_usage(char* app_name) {
    printf("\nUsage: %s FILE_IN FILE_OUT EXECUTION_MODE ...ARGS\n", app_name);
    printf("\tFILE_IN: input image file name\n\tFILE_OUT: output file name\n");
    printf("\tEXECUTION_MODE: one of 'filter', 'histogram', 'stretch', 'equalize', 'gradient', 'nonmax', 'hysteresis', 'segment'\n");
    printf("\nIf EXECUTION_MODE is 'filter':\n");
    printf("\t%s FILE_IN FILE_OUT filter FILTER_TYPE FILTER_PADDING TIMES DIMENSION\n", app_name);
    printf("\t\tFILTER_TYPE: type of filter, one of 'gaussian', 'average', 'mean'\n");
    printf("\t\tFILTER_PADDING: type of filter padding, one of 'zero', 'maxval', 'mean', 'random'\n");
    printf("\t\tTIMES: number of times to apply the filter\n");
    printf("\t\tDIMENSION: dimension of filter, odd number\n");
    printf("\nIf EXECUTION_MODE is 'histogram':\n");
    printf("\t%s FILE_IN FILE_OUT histogram\n", app_name);
    printf("\nIf EXECUTION_MODE is 'stretch':\n");
    printf("\t%s FILE_IN FILE_OUT stretch\n", app_name);
    printf("\nIf EXECUTION_MODE is 'equalize':\n");
    printf("\t%s FILE_IN FILE_OUT equalize\n", app_name);
    printf("\nIf EXECUTION_MODE is 'gradient':\n");
    printf("\t%s FILE_IN FILE_OUT gradient GRADIENT_TYPE FILTER_PADDING GRADIENT_THRESHOLD\n", app_name);
    printf("\t\tGRADIENT_TYPE: type of gradient filter, one of 'roberts', 'prewitt', 'sobel', 'scharr'\n");
    printf("\t\tFILTER_PADDING: type of filter padding, one of 'zero', 'maxval', 'mean', 'random'\n");
    printf("\t\tGRADIENT_THRESHOLD: number, all the intensities below that would be discarded, [0 ... 255]\n");
    printf("\nIf EXECUTION_MODE is 'nonmax':\n");
    printf("\t%s FILE_IN FILE_OUT nonmax GRADIENT_TYPE FILTER_PADDING\n", app_name);
    printf("\t\tGRADIENT_TYPE: type of gradient filter, one of 'roberts', 'prewitt', 'sobel', 'scharr'\n");
    printf("\t\tFILTER_PADDING: type of filter padding, one of 'zero', 'maxval', 'mean', 'random'\n");
    printf("\nIf EXECUTION_MODE is 'hysteresis':\n");
    printf("\t%s FILE_IN FILE_OUT hysteresis GRADIENT_TYPE FILTER_PADDING HIGHER_THRESHOLD LOWER_THRESHOLD\n", app_name);
    printf("\t\tGRADIENT_TYPE: type of gradient filter, one of 'roberts', 'prewitt', 'sobel', 'scharr'\n");
    printf("\t\tFILTER_PADDING: type of filter padding, one of 'zero', 'maxval', 'mean', 'random'\n");
    printf("\t\tHIGHER_THRESHOLD: higher threshold for Cannys' approach, [0 ... 255]\n");
    printf("\t\tLOWER_THRESHOLD: lower threshold for Cannys' approach, [0 ... 255]\n");
    printf("\nIf EXECUTION_MODE is 'segment':\n");
    printf("\t%s FILE_IN FILE_OUT segment SEGMENT_CORDINALITY SEGMENT_CLUSTERS\n", app_name);
    printf("\t\tSEGMENT_CORDINALITY: cordinality of segmentation, one of 'intensity', 'intensity-location', 'rgb', 'rgb-location'\n");
    printf("\t\tSEGMENT_CLUSTERS: number of clusters for segmentation, natural number\n");
    printf("\n");
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
    gradient_all(gradient_type, padding_type, maxval, width, height, bytemap);

    for (int i = 0; i < width * height; i++) bytemap[i] = bytemap[i] > threshold ? bytemap[i] : 0;
    write_pgm(output_file, width, height, maxval, bytemap);
}

void nonmax(char* gradient_type, char* padding_type, char* input_file, char* output_file) {
    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);
    non_max_suppress(gradient_type, padding_type, maxval, width, height, bytemap);
    write_pgm(output_file, width, height, maxval, bytemap);
}

void hysteresis(char* gradient_type, char* padding_type, char* input_file, char* output_file, byte higher, byte lower) {
    int version, height, width, maxval = 255;
    byte* bytemap = read_pgm(input_file, &version, &width, &height, &maxval);
    hysteresis_threshold(gradient_type, padding_type, maxval, width, height, bytemap, higher, lower);
    write_pgm(output_file, width, height, maxval, bytemap);
}

void segment(char* cordinality, int clusters, char* input_file, char* output_file) {
    if (clusters <= 0) pm_erreur("Clusters number should be natural");

    int version, height, width, maxval = 255;

    byte* bytemap;
    if (strcmp(cordinality, "intensity") == 0 || strcmp(cordinality, "intensity-location") == 0) {
        bytemap = read_pgm(input_file, &version, &width, &height, &maxval);
        if (strcmp(cordinality, "intensity") == 0) intencity_segmentation(clusters, maxval, width * height, bytemap);
        else intencity_location_segmentation(clusters, maxval, width, height, bytemap);
        write_pgm(output_file, width, height, maxval, bytemap);
    } else if (strcmp(cordinality, "rgb") == 0 || strcmp(cordinality, "rgb-location") == 0) {
        bytemap = read_ppm(input_file, &version, &width, &height, &maxval);
        if (strcmp(cordinality, "rgb") == 0) rgb_segmentation(clusters, maxval, width * height, bytemap);
        else rgb_location_segmentation(clusters, maxval, width, height, bytemap);
        write_ppm(output_file, width, height, maxval, bytemap);
    } else pm_erreur("Unknown segmentation cordinality");
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
    } else if (strcmp(argv[3], "nonmax") == 0) {
        if (argc != 6) pm_erreur("Wrong argument number for image nonmax suppression");
        else nonmax(argv[4], argv[5], argv[1], argv[2]);
    } else if (strcmp(argv[3], "hysteresis") == 0) {
        if (argc != 8) pm_erreur("Wrong argument number for image hysteresis thresholding");
        else hysteresis(argv[4], argv[5], argv[1], argv[2], atoi(argv[6]), atoi(argv[7]));
    } else if (strcmp(argv[3], "segment") == 0) {
        if (argc != 6) pm_erreur("Wrong argument number for image segmentation");
        else segment(argv[4], atoi(argv[5]), argv[1], argv[2]);
    } else pm_erreur("Wrong execution mode");

    return EXIT_SUCCESS;
}
