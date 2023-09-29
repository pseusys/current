#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gaussian.h"
#include "average.h"
#include "mean.h"

#include "../../viscomp-lab1/Util.h"


int print_usage(char* app_name) {
    printf("\nUsage: %s FILE_IN FILE_OUT EXECUTION_MODE ...ARGS\n", app_name);
    printf("\tFILE_IN: input image file name\n\tFILE_OUT: output file name\n");
    printf("\tEXECUTION_MODE: one of 'filter' or 'histogram'\n");
    printf("\nIf EXECUTION_MODE is 'filter':\n");
    printf("\t%s FILE_IN FILE_OUT filter FILTER_TYPE TIMES DIMENSION\n", app_name);
    printf("\t\tFILTER_TYPE: type of filter, one of 'gaussian', 'average', 'mean'\n");
    printf("\t\tTIMES: number of times to apply the filter\n");
    printf("\t\tDIMENSION: dimension of filter, odd number\n");
    printf("\nIf EXECUTION_MODE is 'histogram':\n");
    printf("\t...\n");
    return EXIT_SUCCESS;
}

int filter(char* filter_type, int times, int dimension, FILE* input_file, FILE* output_file) {
    byte* bytemap;
    int version, rows, cols, maxval = 255;

    if (dimension <= 0 || dimension % 2 == 0 || times <= 0) {
        printf("Filter dimension should be a positive odd number, times should be a positive number!\n");
        return EXIT_FAILURE;
    }

    /*  Magic number reading */
    getc(input_file);
    version = getc(input_file);
    if (version == EOF) pm_erreur("EOF / read error / magic number");
    if (version != '5') pm_erreur("Wrong file type");

    /* Reading image dimensions */
    cols = pm_getint(input_file);
    rows = pm_getint(input_file);
    maxval = pm_getint(input_file);

    /* Memory allocation  */
    bytemap = (byte*) malloc(cols * rows * sizeof(byte));

    /* Reading */
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            bytemap[i * cols + j] = pm_getrawbyte(input_file);

    /* Filtering */
    if (strcmp(filter_type, "gaussian") == 0) filter_all_gaussian(cols, rows, times, dimension, bytemap);
    else if (strcmp(filter_type, "average") == 0) filter_all_average(cols, rows, times, dimension, bytemap);
    else if (strcmp(filter_type, "mean") == 0) filter_all_mean(cols, rows, times, dimension, bytemap);
    else {
        printf("Unknown filtering type: %s\n", filter_type);
        return EXIT_FAILURE;
    }

    /* Writing */
    fprintf(output_file, "P5\n%d %d\n%d\n", cols, rows, maxval);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols ; j++)
            fprintf(output_file, "%c",bytemap[i * cols + j]);
    
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    FILE* input_file, * output_file;
    if (argc < 4) return print_usage(argv[0]);

    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        printf("Error opening file %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    int result;
    if (strcmp(argv[3], "filter") == 0) {
        if (argc != 7) return print_usage(argv[0]);
        else result = filter(argv[4], atoi(argv[5]), atoi(argv[6]), input_file, output_file);
    } else if (strcmp(argv[3], "histogram") == 0) {
        result = EXIT_FAILURE;
    }

    fclose(input_file);
    fclose(output_file);
    return result;
}
