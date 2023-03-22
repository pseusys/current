#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

double experiments[NBEXPERIMENTS];

double average_time() {
    double s = 0;
    for (uint64_t i = 0; i < NBEXPERIMENTS; i++) s = s + experiments[i];
    return s / NBEXPERIMENTS;
}

/* 
   Merge two sorted chunks of array T!
   The two chunks are of size size
   First chunck starts at T[0], second chunck starts at T[size]
*/
void merge(uint64_t* T, const uint64_t size) {
  uint64_t* X = (uint64_t*) malloc (2 * size * sizeof(uint64_t)) ;
  
  uint64_t i = 0;
  uint64_t j = size;
  uint64_t k = 0;

  while ((i < size) && (j < 2*size)) {
    if (T[i] < T[j]) {
      X[k] = T[i];
      i = i + 1;
    } else {
      X[k] = T[j];
      j = j + 1;
    }
    k = k + 1;
  }

  if (i < size) memcpy(X + k, T + i, (size - i) * sizeof(uint64_t));
  else memcpy(X + k, T + j, (2 * size - j) * sizeof(uint64_t));
  
  memcpy(T, X, 2*size*sizeof(uint64_t));
  free(X);
}
