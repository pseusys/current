#ifndef VISCOMP_LAB_2_IMAGINE_H
#define VISCOMP_LAB_2_IMAGINE_H

#include <stdio.h>

#include "../../viscomp-lab1/Util.h"

/**
 * GENERATES: return value
*/
FILE* svg_header(char* file_name, int svg_width, int svg_height);

/**
 * CONSUMES: file
*/
void svg_footer(FILE* file);

void svg_line(FILE* file, double x1, double y1, double x2, double y2, char* s);

void svg_rect(FILE* file, double x, double y, double w, double h, char* s, char* f);

/**
 * GENERATES: return value
*/
byte* read_pgm(char* input, int* version, int* width, int* height, int* maxval);

/**
 * CONSUMES: bytemap
*/
void write_pgm(char* output, int width, int height, int maxval, byte* bytemap);

#endif // VISCOMP_LAB_2_IMAGINE_H
