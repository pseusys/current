#ifndef VISCOMP_LAB_1_UTILS_H
#define VISCOMP_LAB_1_UTILS_H

/* Headers for PBM+ */
/* E.B.             */

#include <stdio.h>

typedef unsigned char byte;
typedef unsigned char gray;

char pm_getc(FILE* file);
byte pm_getbit(FILE* file);
unsigned char pm_getrawbyte(FILE* file);
int pm_getint( FILE* file);

void pm_erreur(char *);

#endif // VISCOMP_LAB_1_UTILS_H
