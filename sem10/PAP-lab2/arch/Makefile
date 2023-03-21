CC = gcc
CFLAGS = -O2 -fopenmp
LDFLAGS = -fopenmp

HEADER_FILES = $(wildcard sources/*.h)

RAND_INIT = 0
THREADS = 8
TIMES = 12

ifeq ($(RAND_INIT), 1)
$(Initialization of the vector with random values)
CONFIG_FLAGS += -DRINIT
endif



build/%.o: sources/%.c $(HEADER_FILES)
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

build/%.run: build/%.o build/utils.o
	@$(CC) $(LDFLAGS) -o $@ $^

renew-%:
	@mkdir -p ./build
	@rm -f build/%.run 

clean:
	@rm -rf build



bubble: renew-bubble build/bubble.run
	build/bubble.run $(TIMES) $(THREADS)

mergesort: renew-mergesort build/mergesort.run
	build/mergesort.run $(TIMES) $(THREADS)

odd-even: renew-odd-even build/odd-even.run
	build/odd-even.run $(TIMES) $(THREADS)

quicksort: renew-quicksort build/quicksort.run
	build/quicksort.run $(TIMES) $(THREADS)

all: bubble mergesort odd-even quicksort renew-%



.PHONY: prepare rm-% clean bubble mergesort even-odd quicksort all
