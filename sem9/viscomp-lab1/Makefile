.PHONY: all build rebuild clean pgmtopgm ppmto3pgm

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=

PGMTOPGM_INPUT=mandrill.pgm
PGMTOPGM_OUTPUT=mandrill_alt.pgm
PPMTO3PGM_INPUT=mandrill.ppm
PPMTO3PGM_OUTPUT=mandrill_red.pgm mandrill_green.pgm mandrill_blue.pgm

all: build

build: build/pgmtopgm.run build/ppmto3pgm.run

rebuild: clean build

pgmtopgm: build/pgmtopgm.run
	@ ./$^ $(PGMTOPGM_INPUT) $(PGMTOPGM_OUTPUT)

ppmto3pgm: build/ppmto3pgm.run
	@ ./$^ $(PPMTO3PGM_INPUT) $(PPMTO3PGM_OUTPUT)

build/%.run: build/%.o build/Util.o Util.h
	@ $(CC) -o $@ $^ $(LDFLAGS)

build/%.o: %.c
	@ mkdir -p build
	@ $(CC) -o $@ -c $< $(CFLAGS)

clean:
	@ rm -rf build $(PGMTOPGM_OUTPUT) $(PPMTO3PGM_OUTPUT)
