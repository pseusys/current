.ONESHELL:
.EXPORT_ALL_VARIABLES:

THREADS=`nproc --all`
ALGORITHM=-1
TIMES=12

RAND_INIT=1
VERBOSE=1
QUICK_FAIL=0

NBEXPERIMENTS=10
MAXTASKSPERTHREAD=3

UTILS = $(wildcard sources/utils_*.c)



build/%.run: sources/%.c $(UTILS) sources/utils.h
	@ # Compile and run source file.
	rm -f $@ 2> /dev/null
	gcc -DRINIT=$(RAND_INIT) -DVERB=$(VERBOSE) -DQFAIL=$(QUICK_FAIL) -DNBEXP=$(NBEXPERIMENTS) -DMTPTH=$(MAXTASKSPERTHREAD) -O2 -fopenmp -o $@ $^

prepare:
	@
	mkdir -p build
	mkdir -p test
.PHONY: prepare

clean:
	@
	rm -rf build
	rm -rf test
.PHONY: clean



autorun: prepare
	@
	echo "Running '$(ALGO)' algorithm..." 1>&2
	make -s build/$(ALGO).run
	build/$(ALGO).run $(TIMES) $(THREADS) $(ALGORITHM) || { if [ $$QUICK_FAIL = 1 ]; then exit 1; fi }
	echo "Run '$(ALGO)' algorithm!" 1>&2
.PHONY: autorun

autotest: prepare
	@
	echo "Testing '$(ALGO)' algorithm..." 1>&2
	bash autotest.sh $(ALGO)
	echo "Tested '$(ALGO)' algorithm!" 1>&2
.PHONY: autotest
