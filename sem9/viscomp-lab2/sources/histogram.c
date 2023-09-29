#include <stdlib.h>
#include <string.h>

#include "histogram.h"
#include "imagine.h"


int svg_size = 1024;

char* svg_back_color = "burlywood";
char* svg_border_color = "black";
char* svg_none_color = "none";


/**
 * GENERATES: return value
*/
char* svg_color(byte value) {
    char* color = calloc(17, sizeof(char));
    sprintf(color, "rgb(%d,%d,%d)", value, value, value);
    return color;
}

long long int* calculate_histogram(int width, int height, int maxval, byte* source) {
    long long int* container = (long long int*) calloc(maxval, sizeof(long long int));
    for (int i = 0; i < height * width; i++) container[source[i]]++;
    return container;
}

void write_histogram(char* output, int size, long long int* container) {
    long long int maximum = 0;
    for (int i = 0; i < size; i++)
        if (container[i] > maximum)
            maximum = container[i];

    int chart = (svg_size / 5) * 4;
    int margin = chart / 10;

    int full_size = chart + margin * 2;
    FILE* output_file = svg_header(output, full_size, full_size);
    svg_rect(output_file, 0, 0, full_size, full_size, svg_none_color, svg_back_color);

    for (int i = 0; i < size; i++) {
        double width = (double) chart / size;
        double height = ((double) container[i] / maximum) * chart;
        char* color = svg_color(((double) i / size) * 255);
        svg_rect(output_file, i * width + margin, chart - height + margin, width, height, svg_border_color, color);
        free(color);
    }

    svg_footer(output_file);
    free(container);
}
