.ONESHELL:
.EXPORT_ALL_VARIABLES:


CC = mpicc
CONFIG_FLAGS =

HEADER_FILES = $(wildcard libs/*.h)

EXTRA_SRC = $(wildcard libs/*.c)

RAND_INIT=1

CHECK_CORRECT=1

EVAL_PERF=1

THREADS=`nproc --all`

MATRIX_SIZE=$(THREADS)

EXPERIMENTS=5

SILENT=0
CLEAN=
EXEC=


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


build/%.run: src/%.c $(EXTRA_SRC) $(HEADER_FILES)
	@
	mkdir -p $(@D)
	$(CC) -o $@ $^ -lm $(CONFIG_FLAGS)

clean:
	@
	test -n "$(CLEAN)" && {
		rm -f ./build/$(CLEAN).run
	} || {
		rm -rf ./build
	}
.PHONY: clean

build_and_run:
	@
	test -n "$(EXEC)" || { echo "Executable not defined (env var 'EXEC')!"; exit 1; }
	make --silent SILENT=1 CLEAN=$(EXEC) clean
	make --silent SILENT=1 build/$(EXEC).run
	mpirun -np $(THREADS) --oversubscribe ./build/$(EXEC).run $(MATRIX_SIZE)
.PHONY: build_and_run


sequential:
	@
	make --silent SILENT=1 EXEC=$@ build_and_run
.PHONY: sequential

matrix-vector:
	@
	make --silent SILENT=1 EXEC=$@ build_and_run
.PHONY: matrix-vector

matrix-matrix:
	@
	make --silent SILENT=1 EXEC=$@ build_and_run
.PHONY: matrix-matrix

fox:
	@
	make --silent SILENT=1 EXEC=$@ build_and_run
.PHONY: fox

canon:
	@
	make --silent SILENT=1 EXEC=$@ build_and_run
.PHONY: canon

autotest: sequential matrix-vector matrix-matrix
	mkdir -p test
	bash autotest.sh 
.PHONY: autotest

venv:
	python3 -m venv venv
	pip3 install --upgrade pip
	pip3 install matplotlib~=3.7.1 pandas~=2.0.0

generate_test_graphs: venv autotest
	python3 -m plotting -i test/matrix_size.csv -x "Matrix size (n * 5n)" -y "Execution time (ns)" -t "Performance test (matrix size)" -l x
	python3 -m plotting -i test/threads_number.csv -x "Threads number" -y "Execution time (ns)" -t "Performance test (threads number)"
.PHONY: generate_test_graphs

