#include "alter.h"
#include "histogram.h"

#include <stdio.h>


void stretch_histogram(int width, int height, int maxval, byte* source) {
    int minimum = maxval, maximum = 0;
    for (int i = 0; i < height * width; i++) {
        if (source[i] < minimum) minimum = source[i];
        if (source[i] > maximum) maximum = source[i];
    }
    for (int i = 0; i < height * width; i++) source[i] = maxval * (source[i] - minimum) / (maximum - minimum);
}

void equalize_histogram(int width, int height, int maxval, byte* source) {
    long long int* cumulative = calculate_histogram(width, height, maxval, source);
    long long int cumsum = cumulative[0];

    for (int i = 1; i < maxval; i++) {
        cumulative[i] += cumsum;
        cumsum = cumulative[i];
    }

    for (int i = 0; i < height * width; i++) source[i] = maxval * cumulative[source[i]] / cumsum;
}
