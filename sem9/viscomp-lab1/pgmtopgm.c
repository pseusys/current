#include <stdlib.h>
#include <stdio.h>
#include "Util.h"



int main(int argc, char* argv[])
    {
    FILE *ifp, *ofp;
    gray* graymap;
    int ich1, ich2, rows, cols, maxval=255, pgmraw_in, pgmraw_out;
    int i, j;

    /* Arguments */
    if ( argc != 3 ){
      printf("\nUsage: %s file_in file_out \n\n", argv[0]);
      exit(0);
    }

    /* Opening input file */
    ifp = fopen(argv[1],"r");
    if (ifp == NULL) {
      printf("error in opening file %s\n", argv[1]);
      exit(1);
    }

    /*  Magic number reading */
    ich1 = getc( ifp );
    if ( ich1 == EOF ) {
        pm_erreur( "EOF / read error / magic number" );
    }
    ich2 = getc( ifp );
    if ( ich2 == EOF ) {
        pm_erreur( "EOF /read error / magic number" );
    }
  
    if(ich2 != '2' && ich2 != '5') {
        pm_erreur(" wrong file type ");
    }
    else {
      if(ich2 == '2') {
        pgmraw_in = 0;
      }
      else {
        pgmraw_in = 1;
      }
    }

    /* Reading image dimensions */
    cols = pm_getint( ifp );
    rows = pm_getint( ifp );
    maxval = pm_getint( ifp );

    /* Memory allocation  */
    graymap = (gray *) malloc(cols * rows * sizeof(gray));

    /* Reading */
    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++)
        if(pgmraw_in) {
          graymap[i * cols + j] = pm_getrawbyte(ifp) ;
        }
        else {
          graymap[i * cols + j] = pm_getint(ifp);
        }
    
    /* Closing input file */
    fclose(ifp);
    
    /* Opening output file */
    ofp = fopen(argv[2],"w");
    if (ofp == NULL) {
      printf("error in opening file %s\n", argv[2]);
      exit(1);
    }

    /* Writing */
    // pgmraw_in is the opposite of pgmraw_out
    if (pgmraw_in) {
      pgmraw_out = 0;
    }
    else {
      pgmraw_out = 1;
    }
      
    if(pgmraw_out)
      fprintf(ofp, "P5\n");
    else
      fprintf(ofp, "P2\n");

    fprintf(ofp, "%d %d \n", cols, rows);
    fprintf(ofp, "%d\n", maxval);

    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++)
        if(pgmraw_out)
          fprintf(ofp, "%c",graymap[i * cols + j]);
        else
          fprintf(ofp, "%d ",graymap[i * cols + j]);
    
    /* Closing output file */
    fclose(ofp);
    
    return 0;
}
