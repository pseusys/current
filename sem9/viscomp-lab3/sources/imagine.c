#include <stdlib.h>

#include "imagine.h"


FILE* svg_header(char* file_name, int svg_width, int svg_height) {
    FILE* input_file = fopen(file_name, "w");
    if (input_file == NULL) pm_erreur("Error opening histogram file");
    fprintf(input_file, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n\t<defs>\n\t\t<marker id=\"head\" orient=\"auto\" markerWidth=\"5\" markerHeight=\"4\" refX=\"0\" refY=\"2\">\n\t\t\t<path d=\"M0,0 V4 L5,2 Z\" fill=\"black\" />\n\t\t</marker>\n\t</defs>", svg_width, svg_height);
    return input_file;
}

void svg_footer(FILE* file) {
    fprintf(file, "</svg>\n");
    fclose(file);
}

void svg_number(FILE* file, double x, double y, long long int s) {
    fprintf(file, "\t<text x=\"%lf\" y=\"%lf\" style=\"font-size: 20px; text-anchor: end;\">%lld</text>\n", x, y, s);
}

void svg_arrow(FILE* file, double x1, double y1, double x2, double y2) {
    fprintf(file, "\t<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"black\" marker-end=\"url(#head)\" style=\"stroke-width:2.5;\"/>\n", x1, y1, x2, y2);
}

void svg_rect(FILE* file, double x, double y, double w, double h, char* f) {
    fprintf(file, "\t<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" fill=\"%s\" stroke=\"black\" style=\"stroke-width:0.25;\"/>\n", x, y, w, h, f);
}

byte* read_pgm(char* input, int* version, int* width, int* height, int* maxval) {
    FILE* input_file = fopen(input, "r");
    if (input_file == NULL) pm_erreur("Error opening input file");

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
    if (output_file == NULL) pm_erreur("Error opening output file");

    fprintf(output_file, "P5\n%d %d\n%d\n", width, height, maxval);

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width ; j++)
            fprintf(output_file, "%c", bytemap[i * width + j]);

    fclose(output_file);
    free(bytemap);
}
