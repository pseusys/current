CC = gcc
CFLAGS = -O2 -fopenmp
LDFLAGS = -fopenmp

EXEC = 	build/bubble.run	\
	build/mergesort.run	    \
	build/quicksort.run	    \
	build/odd-even.run

HEADER_FILES = $(wildcard sources/*.h)

RAND_INIT = 0
TIMES = 15

ifeq ($(RAND_INIT), 1)
$(Initialization of the vector with random values)
CONFIG_FLAGS += -DRINIT
endif



build/%.o: sources/%.c $(HEADER_FILES)
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

build/%.run: build/%.o build/utils.o
	@$(CC) $(LDFLAGS) -o $@ $^

prepare:
	@mkdir -p build

rm-%: build/%.run
	@rm -f build/%.run 

clean:
	@rm -rf build



bubble: rm-bubble prepare build/bubble.run
	build/bubble.run $(TIMES)

mergesort: rm-mergesort prepare build/mergesort.run
	build/mergesort.run $(TIMES)

odd-even: rm-odd-even prepare build/odd-even.run
	build/odd-even.run $(TIMES)

quicksort: rm-quicksort prepare build/quicksort.run
	build/quicksort.run $(TIMES)

all: bubble mergesort odd-even quicksort



.PHONY: prepare rm-% clean bubble mergesort even-odd quicksort all
