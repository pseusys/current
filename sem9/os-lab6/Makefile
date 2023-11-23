.ONESHELL:
SHELL = /bin/bash

CC = gcc

CFLAGS = -g -Wall -Werror -O3

## picky compiler options
# CFLAGS += -Wall -Wextra -Wpointer-arith -pedantic -g -std=gnu99

LDFLAGS = -lpthread

## add the memory sanitizer
CFLAGS += -fsanitize=address
LDFLAGS += -fsanitize=address

# defines the set of configuration variables for the Makefile
include Makefile.config

SRC_APPS = $(wildcard tests/*.c)

TARGETS = $(patsubst %.c, %.run, $(SRC_APPS))

SRC_DEPS = $(wildcard *.c)

SRC_DEPS_OBJ= $(patsubst %.c, %.o, $(SRC_DEPS))

DEPS = $(wildcard *.h)

DEPS += Makefile.config


ifdef VERBOSE
ifneq ($(VERBOSE),0)
CONFIG_FLAGS += -DVERBOSE=$(VERBOSE)
endif
endif

ifdef WITH_DEPENDENCIES
ifneq ($(WITH_DEPENDENCIES),0)
CONFIG_FLAGS += -DWITH_DEPENDENCIES
endif
endif

CONFIG_FLAGS += -DQUEUE_SIZE=$(QUEUE_SIZE)
CONFIG_FLAGS += -DTHREAD_COUNT=$(THREAD_COUNT)

# input argument
ARG = 10



all: $(TARGETS)

tests/%.run: tests/%.o $(SRC_DEPS_OBJ)
	@
	$(CC) -o $@ $^ $(LDFLAGS)

tests/%.o: tests/%.c $(DEPS)
	@
	$(CC) -c $< $(CFLAGS) $(CONFIG_FLAGS) -o $@

%.o: %.c $(DEPS)
	@
	$(CC) -c $< $(CFLAGS) $(CONFIG_FLAGS)


tests/%.eval: tests/%.run
	make --no-print-directory -s -B $< VERBOSE=0 1>/dev/null 2>&1
	./$< $(ARG)
.PHONY: tests/%.eval

tests/%.perf: tests/%.run
	make --no-print-directory -s -B $< VERBOSE=0 1>/dev/null 2>&1
	time ./$< $(ARG)
.PHONY: tests/%.eval

test_all: $(TARGETS)
	@ # Run all the tests together
	for test in $^; do
		echo -e "\t\e[33;1m**** Test $$test *****\e[0m"
		make --no-print-directory -s -B $$test VERBOSE=0 1>/dev/null 2>&1
		OUTPUT=`./$$test 2>&1`
		if [[ "$$OUTPUT" =~ ^Usage:.* ]]; then OUTPUT=`./$$test $(ARG) 2>&1`; fi
		echo "$$OUTPUT"
	done
.PHONY: test_all


clean:
	@
	rm -rf *.o tests/*.o tests/*.run *~ tests/*~
