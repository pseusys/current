#include <stdlib.h>
#include <stdio.h>
#include "Util.h"



int main(int argc, char* argv[]) {
    byte* colormap;
    FILE *ifp, *ofp;
    int ich1, ich2, rows, cols, maxval=255, ascii_format;

    /* Arguments */
    if (argc != 5) {
        printf("Usage: %s file_in file_red_out file_green_out file_blue_out\n\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* Opening input file */
    ifp = fopen(argv[1], "r");
    if (ifp == NULL) {
        printf("Error opening file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /*  Magic number reading */
    ich1 = getc(ifp);
    if (ich1 == EOF) pm_erreur("EOF / read error / magic number");
    ich2 = getc(ifp);
    if (ich2 == EOF) pm_erreur("EOF / read error / magic number");
  
    if (ich2 != '3' && ich2 != '6') pm_erreur("wrong file type");
    else ascii_format = ich2 == '3';

    /* Reading image dimensions */
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);

    /* Memory allocation  */
    colormap = (byte*) malloc(cols * rows * 3 * sizeof(byte));

    /* Reading */
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols ; j++)
            for (int k = 0; k < 3 ; k++)
                if (ascii_format) colormap[(i * cols + j) * 3 + k] = pm_getint(ifp);
                else colormap[(i * cols + j) * 3 + k] = pm_getrawbyte(ifp) ;
    
    /* Closing input file */
    fclose(ifp);
    
    for (int k = 0; k < 3 ; k++) {
        /* Opening output file */
        ofp = fopen(argv[2 + k], "w");
        if (ofp == NULL) {
            printf("error in opening file %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        /* Writing */
        if (ascii_format) fprintf(ofp, "P2\n");
        else fprintf(ofp, "P5\n");

        fprintf(ofp, "%d %d\n", cols, rows);
        fprintf(ofp, "%d\n", maxval);

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols ; j++)
                if (ascii_format) fprintf(ofp, "%d ",colormap[(i * cols + j) * 3 + k]);
                else fprintf(ofp, "%c",colormap[(i * cols + j) * 3 + k]);

        /* Closing output file */
        fclose(ofp);
    }

    free(colormap);
    return EXIT_SUCCESS;
}
