.PHONY: all rerun run clean

SOURCES=main maze svg ppm
OBJECTS=$(foreach obj,$(SOURCES),build/$(obj).o)
HEADERS=$(foreach hed,$(SOURCES),$(hed).h)

W=1024
H=1024
F=ppm

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=

all: run

rerun: clean run

run: maze.run
	@ ./$^ $(W) $(H) $(F)

maze.run: $(OBJECTS) $(HEADERS)
	@ $(CC) -o $@ $^ $(LDFLAGS)

build/%.o: %.c
	@ mkdir -p build
	@ $(CC) -o $@ -c $< $(CFLAGS)

clean:
	@ rm -rf build
