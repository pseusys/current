.ONESHELL:
.EXPORT_ALL_VARIABLES:


CC = mpicc
CFLAGS = 
LDFLAGS =
CONFIG_FLAGS =

HEADER_FILES = $(wildcard src/*.h)

EXTRA_SRC = $(wildcard src/*.c)

RAND_INIT=1

CHECK_CORRECT=1

EVAL_PERF=1

THREADS=`nproc --all`

MATRIX_SIZE=$(THREADS)

EXPERIMENTS=5

SILENT=0


ifeq ($(EVAL_PERF), 0)
ifeq ($(CHECK_CORRECT), 0)
ifeq ($(SILENT), 0)
$(error Wrong combination of options. EVAL_PERF or CHECK_CORRECT need to be set)
endif
endif
endif

ifeq ($(RAND_INIT), 1)
ifeq ($(SILENT), 0)
$(info Initialization of the matrices is random)
endif
CONFIG_FLAGS += -DRINIT
endif

ifeq ($(CHECK_CORRECT), 1)
ifeq ($(SILENT), 0)
$(info Computation if checked for correctness)
endif
CONFIG_FLAGS += -DCHECK_CORRECTNESS
endif

ifeq ($(EVAL_PERF), 1)
ifeq ($(SILENT), 0)
$(info Performance is evaluated, experiments number: $(EXPERIMENTS))
endif
CONFIG_FLAGS += -DPERF_EVAL -DEXPERIMENTS=$(EXPERIMENTS)
endif


%.run: %.c $(EXTRA_SRC) $(HEADER_FILES)
	$(CC) -o $@ $^ $(CONFIG_FLAGS)

clean:
	rm -f *.run
.PHONY: clean


sequential:
	@
	make --silent SILENT=1 clean
	make --silent SILENT=1 $@.run
	mpirun -np $(THREADS) --oversubscribe ./$@.run $(MATRIX_SIZE)
.PHONY: sequential

matrix-matrix:
	@
	make --silent SILENT=1 clean
	make --silent SILENT=1 $@.run
	mpirun -np $(THREADS) --oversubscribe ./$@.run $(MATRIX_SIZE)
.PHONY: sequential
