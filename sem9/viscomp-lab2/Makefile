.ONESHELL:
.PHONY: all build rebuild clean gaussian_16 test

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=

FILTER_TIMES=1
FILTER_DIMENSION=101
FILTER_TYPE=average
GAUSSIAN_16_INPUT=boat_noise2.pgm
GAUSSIAN_16_OUTPUT=denoised

all: build

build: build/gaussian_16.run

rebuild: clean build

test: build/gaussian_16.run
	@ $(foreach var,$(shell seq 3 2 13),./$^ $(GAUSSIAN_16_INPUT) $(GAUSSIAN_16_OUTPUT)_$(var).pgm $(FILTER_TYPE) 1 $(var);)

gaussian_16: build/gaussian_16.run
	@ ./$^ $(GAUSSIAN_16_INPUT) $(GAUSSIAN_16_OUTPUT)_$(FILTER_DIMENSION).pgm $(FILTER_TYPE) $(FILTER_TIMES) $(FILTER_DIMENSION)

build/%.run: build/%.o build/Util.o ../viscomp-lab1/Util.h
	@ $(CC) -o $@ $^ $(LDFLAGS) -lm

build/Util.o: ../viscomp-lab1/Util.c
	@ mkdir -p build
	@ $(CC) -o $@ -c $< $(CFLAGS) -lm

build/%.o: %.c
	@ mkdir -p build
	@ $(CC) -o $@ -c $< $(CFLAGS) -lm

clean:
	@ rm -rf build $(GAUSSIAN_16_OUTPUT)_*.pgm
