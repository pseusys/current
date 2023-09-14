#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG = pgmtopgm ppmto3pgm

all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

#pxmtopxm : pxmtopxm.o Util.o

pgmtopgm : pgmtopgm.o Util.o

ppmto3pgm : ppmto3pgm.o Util.o

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

