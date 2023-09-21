#include <stdio.h>
#include <stdlib.h>

static unsigned long long X = 123456ULL;

unsigned char crand48(void) {
  unsigned long long int a = 0x5DEECE66D, c = 0xB, m = 0xFFFFFFFFFFFF;
  X = (a * X + c) & m;
  return (X & 0x00FF00000000) >> 32;
}

int main(void) {
  printf("%hhu\n", crand48());
  printf("%hhu\n", crand48());
  printf("%hhu\n", crand48());

  return EXIT_SUCCESS;
}
