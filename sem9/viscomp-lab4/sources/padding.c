#include <stdlib.h>
#include <string.h>

#include "padding.h"


/**
 * GENERATES: return value
*/
byte* zero_padding(int size) {
    return (byte*) calloc(size, sizeof(byte));
}

/**
 * GENERATES: return value
*/
byte* maxval_padding(int size, int maxval) {
    byte* padded = (byte*) malloc(size * sizeof(byte));
    memset(padded, maxval, size);
    return padded;
}

/**
 * GENERATES: return value
*/
byte* mean_padding(int size, int maxval) {
    return maxval_padding(size, maxval / 2);
}

/**
 * GENERATES: return value
*/
byte* random_padding(int size, int maxval) {
    byte* padded = (byte*) malloc(size * sizeof(byte));
    for (int i = 0; i < size; i++) padded[i] = rand() % maxval;
    return padded;
}

byte* get_padded(char* padding_type, int size, int maxval) {
    if (strcmp(padding_type, "zero") == 0) return zero_padding(size);
    else if (strcmp(padding_type, "maxval") == 0) return maxval_padding(size, maxval);
    else if (strcmp(padding_type, "mean") == 0) return mean_padding(size, maxval);
    else if (strcmp(padding_type, "random") == 0) return random_padding(size, maxval);
    else pm_erreur("Unknown padding type");
    return NULL;
}
