/* Headers for PBM+ */
/* E.B.             */

typedef unsigned char byte;
typedef unsigned char gray;

char pm_getc(FILE* file);
byte pm_getbit(FILE* file);
unsigned char pm_getrawbyte(FILE* file);
int pm_getint( FILE* file);

void pm_erreur(char *);
