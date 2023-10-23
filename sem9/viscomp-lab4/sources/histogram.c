#include <stdlib.h>
#include <string.h>

#include "histogram.h"
#include "imagine.h"


int svg_size = 1024;
char* svg_back_color = "burlywood";


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
    int legend = chart / 7.5;

    int full_size = chart + margin * 2;
    FILE* output_file = svg_header(output, full_size + legend, full_size + legend);
    svg_rect(output_file, 0, 0, full_size + legend, full_size + legend, svg_back_color);

    svg_arrow(output_file, legend, full_size, legend, margin);
    svg_number(output_file, legend - 20, full_size - margin, 0);
    svg_number(output_file, legend - 20, margin, maximum);
    svg_arrow(output_file, legend, full_size, legend + chart + margin, full_size);
    svg_number(output_file, legend + margin, full_size + 30, 0);
    svg_number(output_file, legend + chart + margin, full_size + 30, size);

    for (int i = 0; i < size; i++) {
        double width = (double) chart / size;
        double height = ((double) container[i] / maximum) * chart;
        char* color = svg_color(((double) i / size) * 255);
        svg_rect(output_file, i * width + margin + legend, chart - height + margin, width, height, color);
        free(color);
    }

    svg_footer(output_file);
    free(container);
}
