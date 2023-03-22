#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

double average_time(double* experiments, uint64_t experiments_length) {
    double s = 0;
    for (uint64_t i = 0; i < experiments_length; i++) s = s + experiments[i];
    return s / experiments_length;
}

uint64_t calculate_max_binary_recursion_power(uint64_t max_tasks, uint64_t current_power, uint64_t current_value) {
    uint64_t new_current = current_value + current_power * 2;
    if (new_current < max_tasks) return calculate_max_binary_recursion_power(max_tasks, current_power + 1, new_current);
    else return current_value;
}

uint64_t calculate_max_binary_recursion_level(uint64_t max_tasks) {
    return calculate_max_binary_recursion_power(max_tasks, 1, 1);
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
