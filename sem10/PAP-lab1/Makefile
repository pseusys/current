CC=gcc
CFLAGS= -fopenmp -g -Wall -O0

UTILS=$(wildcard _*.c _*.h)


build/%.run: %.c
	$(CC) $(CFLAGS) -o $@ $(UTILS) $^

build:
	mkdir build


ex1: build build/ex1.run
	build/ex1.run 8
.PHONY: ex1

ex2: build build/ex2.run
	build/ex2.run 256
.PHONY: ex2


clean:
	rm -rf build
.PHONY: clean
