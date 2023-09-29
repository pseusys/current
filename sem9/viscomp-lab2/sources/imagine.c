#include <stdlib.h>

#include "imagine.h"


FILE* svg_header(char* file_name, int svg_width, int svg_height) {
    FILE* input_file = fopen(file_name, "r");
    if (input_file == NULL) pm_erreur("Error opening file");
    fprintf(input_file, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n", svg_width, svg_height);
    return input_file;
}

void svg_footer(FILE* file) {
    fprintf(file, "</svg>\n");
    fclose(file);
}

void svg_line(FILE* file, double x1, double y1, double x2, double y2, char* s) {
    fprintf(file, "\t<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"%s\" style=\"stroke-width:0.5;\"/>\n", x1, y1, x2, y2, s);
}

void svg_rect(FILE* file, double x, double y, double w, double h, char* s, char* f) {
    fprintf(file, "\t<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" fill=\"%s\" stroke=\"%s\" style=\"stroke-width:0.25;\"/>\n", x, y, w, h, f, s);
}

byte* read_pgm(char* input, int* version, int* width, int* height, int* maxval) {
    FILE* input_file = fopen(input, "r");
    if (input_file == NULL) pm_erreur("Error opening file");

    getc(input_file);
    *version = getc(input_file);
    if (*version == EOF) pm_erreur("EOF / read error / magic number");
    if (*version != '5') pm_erreur("Wrong file type");

    *width = pm_getint(input_file);
    *height = pm_getint(input_file);
    *maxval = pm_getint(input_file);

    byte* bytemap = (byte*) malloc((*width) * (*height) * sizeof(byte));

    for (int i = 0; i < *height; i++)
        for (int j = 0; j < *width; j++)
            bytemap[i * (*width) + j] = pm_getrawbyte(input_file);
    
    fclose(input_file);
    return bytemap;
}

void write_pgm(char* output, int width, int height, int maxval, byte* bytemap) {
    FILE* output_file = fopen(output, "w");
    if (output_file == NULL) pm_erreur("Error opening file");

    fprintf(output_file, "P5\n%d %d\n%d\n", width, height, maxval);

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width ; j++)
            fprintf(output_file, "%c", bytemap[i * width + j]);

    fclose(output_file);
    free(bytemap);
}
