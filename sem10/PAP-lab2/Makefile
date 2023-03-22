.ONESHELL:

THREADS = `nproc --all`
ALGORITHM = -1
TIMES = 12
RAND_INIT = 1
VERBOSE = 1
NBEXPERIMENTS = 10

UTILS = $(wildcard sources/utils_*.c)

ifeq ($(RAND_INIT), 1)
	CONFIG_FLAGS += -DRINIT
endif
ifeq ($(VERBOSE), 1)
	CONFIG_FLAGS += -DVERB
endif



build/%.run: sources/%.c $(UTILS) sources/utils.h
	@ # Compile and run source file.
	mkdir -p build
	gcc $(CONFIG_FLAGS) -O2 -fopenmp -o $@ $^

clean:
	@
	rm -rf build
.PHONY: clean



bubble:
	@
	rm -f build/bubble.run 2> /dev/null
	make --no-print-directory build/bubble.run
	build/bubble.run $(TIMES) $(THREADS) $(ALGORITHM)
.PHONY: bubble

mergesort:
	@
	rm -f buld/mergesort.run 2> /dev/null
	make --no-print-directory build/mergesort.run
	build/mergesort.run $(TIMES) $(THREADS) $(ALGORITHM)
.PHONY: mergesort

odd-even:
	@
	rm -f build/odd-even.run 2> /dev/null
	make --no-print-directory build/odd-even.run
	build/odd-even.run $(TIMES) $(THREADS) $(ALGORITHM)
.PHONY: odd-even

quicksort:
	@
	rm -f build/quicksort.run 2> /dev/null
	make --no-print-directory build/quicksort.run
	build/quicksort.run $(TIMES) $(THREADS) $(ALGORITHM)
.PHONY: quicksort
