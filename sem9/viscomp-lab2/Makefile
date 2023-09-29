.ONESHELL:
.PHONY: all build rebuild help test filter refilter histogram rehistogram clean

SOURCES=main gaussian average mean histogram
OBJECTS=$(foreach obj,$(SOURCES),build/$(obj).o)
HEADERS=$(foreach hed,$(filter-out main,$(SOURCES)),sources/$(hed).h)

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=

FILTER_TIMES=1
FILTER_DIMENSION=11
FILTER_TYPE=average

MAIN_INPUT=boat_noise2.pgm
MAIN_OUTPUT=denoised

all: build

build: build/main.run

rebuild: clean build

help: build/main.run
	@ ./$^

test_filter: build/main.run
	@
	echo "\e[33;1m**** Testing app for $(FILTER_TYPE) filters from 3 to 13 *****\e[0m"
	$(foreach var,$(shell seq 3 2 13),./$^ $(MAIN_INPUT) $(MAIN_OUTPUT)_$(var).pgm filter $(FILTER_TYPE) 1 $(var);)

filter: build/main.run
	@
	echo "\e[33;1m**** Running app for $(FILTER_TYPE) filter of size $(FILTER_DIMENSION) *****\e[0m"
	./$^ $(MAIN_INPUT) $(MAIN_OUTPUT)_$(FILTER_DIMENSION).pgm filter $(FILTER_TYPE) $(FILTER_TIMES) $(FILTER_DIMENSION)

refilter: clean filter

histogram: build/main.run
	@
	echo "\e[33;1m**** Running app for $(FILTER_TYPE) filter of size $(FILTER_DIMENSION) *****\e[0m"
	./$^ $(MAIN_INPUT) $(MAIN_OUTPUT)_$(FILTER_DIMENSION).pgm $(FILTER_TYPE) $(FILTER_TIMES) $(FILTER_DIMENSION)

rehistogram: clean histogram

build/main.run: $(OBJECTS) build/Util.o ../viscomp-lab1/Util.h $(HEADERS)
	@ $(CC) -o $@ $^ $(LDFLAGS) -lm

build/Util.o: ../viscomp-lab1/Util.c
	@ mkdir -p build
	@ $(CC) -o $@ -c $< $(CFLAGS)

build/%.o: sources/%.c
	@ mkdir -p build
	@ $(CC) -o $@ -c $< $(CFLAGS) -lm

clean:
	@ rm -rf build $(MAIN_OUTPUT)_*.pgm
