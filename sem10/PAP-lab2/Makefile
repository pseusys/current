.ONESHELL:
.DEFAULT_GOAL := run

THREADS = `nproc --all`
TIMES = 12
RAND_INIT = 1

UTILS = $(wildcard sources/utils_*.c)



build/%.run: sources/%.c $(UTILS) sources/utils.h
	@ # Compile and run source file.
	mkdir -p build
	rm -f $@ 2> /dev/null
	gcc -O2 -fopenmp -o $@ $^
.PHONY: build/%.run

clean:
	@rm -rf build
.PHONY: clean



bubble: build/bubble.run
	@
	$< $(RAND_INIT) $(TIMES) $(THREADS)
.PHONY: bubble

mergesort: build/mergesort.run
	@
	$< $(RAND_INIT) $(TIMES) $(THREADS)
.PHONY: mergesort

odd-even: build/odd-even.run
	@
	$< $(RAND_INIT) $(TIMES) $(THREADS)
.PHONY: odd-even

quicksort: build/quicksort.run
	@
	$< $(RAND_INIT) $(TIMES) $(THREADS)
.PHONY: quicksort
