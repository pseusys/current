.ONESHELL:
CPU_ARCH=$(shell uname -m)

CC=gcc
LD=ld
PEDANTIC_PARANOID_FREAK =       -g -O0 -Wall -Wshadow -Wcast-align \
				-Waggregate-return -Wstrict-prototypes \
				-Wredundant-decls -Wnested-externs \
				-Wpointer-arith -Wwrite-strings -finline-functions -Werror
REASONABLY_CAREFUL_DUDE =	-Wall -Wpointer-arith
NO_PRAYER_FOR_THE_WICKED =	-w 
WARNINGS = 			$(REASONABLY_CAREFUL_DUDE)


CFLAGS =   -g  $(WARNINGS)
LDFLAGS= 

VERBOSE =	true

# defines the set of configuration variables for the Makefile
include Makefile.config

ifeq ($(ALLOC_POLICY), BF)
ifeq ($(VERBOSE), true)
$(info Using Best Fit policy)
endif
CONFIG_FLAGS += -DBEST_FIT
else ifeq ($(ALLOC_POLICY), WF)
ifeq ($(VERBOSE), true)
$(info Using Worst Fit policy)
endif
CONFIG_FLAGS += -DWORST_FIT
else ifeq ($(ALLOC_POLICY), FF)
ifeq ($(VERBOSE), true)
$(info Using First Fit policy)
endif
CONFIG_FLAGS += -DFIRST_FIT
else ifeq ($(ALLOC_POLICY), NF)
ifeq ($(VERBOSE), true)
$(info Using Next Fit policy)
endif
CONFIG_FLAGS += -DNEXT_FIT
else 
$(error ERROR: using unknown value for ALLOC_POLICY)
endif


ifdef MEM_POOL_SIZE
CONFIG_FLAGS += -DMEM_POOL_SIZE=$(MEM_POOL_SIZE)
endif

ifdef MEM_ALIGNMENT
CONFIG_FLAGS += -DMEM_ALIGNMENT=$(MEM_ALIGNMENT)
endif

# use -DDEBUG=1 to enable debug messages, -DDEBUG=0 to disable them
CONFIG_FLAGS += -DDEBUG=0


MD_FILES = $(wildcard *.md)
HTML_TARGETS = $(patsubst %.md,%.html,$(MD_FILES))

LIBSIM_FILE=libsim.$(CPU_ARCH).so
LIBSIM_NAME=sim.$(CPU_ARCH)

#############################################################################
# Some reminders about automatic variables in Makefiles:
#
#    $@ --> The file name of the target of the rule
#    $< --> The name of the first prerequisite
#    $^ --> The names of all the prerequisites
#############################################################################

all: mem_shell mem_shell_sim

mem_shell: bin/mem_shell

bin/mem_shell: libmalloc.o mem_shell.o
	@$(CC) $(LDFLAGS) -o $@ $^ -ldl

mem_alloc_test: bin/mem_alloc_test

bin/mem_alloc_test: mem_alloc_test.o sources/block_safety.o my_mmap.o
	@$(CC) $(LDFLAGS) $^ -o $@ -ldl

mem_alloc_test.o: sources/mem_alloc.c sources/mem_alloc_types.h my_mmap.h
	@$(CC) -c -DMAIN $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

my_mmap.o: my_mmap.c my_mmap.h
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

mem_shell.o: mem_shell.c
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

#############################################################################

# Notice the presence (and the precise position in the command line) of "-ldl":
#    both are very important because sources/mem_alloc.c uses dlsym
libmalloc.so: libmalloc.o libmalloc_std.o
	@$(CC)  -shared  -Wl,-soname,$@ $^ -o $@ -ldl

libmalloc_std.o:mem_alloc_std.c sources/mem_alloc.h sources/mem_alloc_types.h
	@$(CC) $(CONFIG_FLAGS) $(CFLAGS) -fPIC -c $< -o $@

libmalloc.o: mem_alloc-lib.o my_mmap-lib.o sources/block_safety.o
	@$(LD) -r $^ -o $@

mem_alloc-lib.o: sources/mem_alloc.c sources/mem_alloc_types.h my_mmap.h
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) -fPIC $< -o $@ -ldl

my_mmap-lib.o: my_mmap.c my_mmap.h
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) -fPIC $< -o $@

#############################################################################

sources/%.o: sources/%.c sources/%.h sources/mem_alloc_types.h
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

#############################################################################

test_ls: MEM_POOL_SIZE=1048576
test_ls: libmalloc.so
	@LD_PRELOAD=./libmalloc.so ls
	@LD_PRELOAD=""

test_ps: MEM_POOL_SIZE=1048576
test_ps: libmalloc.so
	@LD_PRELOAD=./libmalloc.so ps
	@LD_PRELOAD=""

#############################################################################

extests/%.run: extests/%.c
	@$(CC) $(LDFLAGS) -o $@ $^

extests/%.test: MEM_POOL_SIZE=4096
extests/%.test: extests/%.run libmalloc.so
	@LD_PRELOAD=./libmalloc.so $<
	@LD_PRELOAD=""

#############################################################################

%.trace: %.in bin/mem_shell
	@cat $< | bin/mem_shell 2>&1 

%.out: %.in bin/mem_shell
	@cat $< | bin/mem_shell 2>&1 | grep -E '^ALLOC|^FREE' >$@

%.out.expected: %.in bin/mem_shell_sim
	@export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
	cat $< | bin/mem_shell_sim ${MEM_POOL_SIZE} ${ALLOC_POLICY} ${MEM_ALIGNMENT} 2>&1 | grep -E '^ALLOC|^FREE' >$@

%.test: %.out %.out.expected
	@if diff $^  >/dev/null; then
	  echo "\e[32m**** Test $@ Passed *****\e[0m"
	else
	  echo "\e[31m**** Test $@ FAILED *****\e[0m"
	  echo "\t Your trace \t vs \t Expected trace"
	  diff -y $^;
	fi

#############################################################################

test_all: VERBOSE=false
test_all:
	@ # Run all the tests together
	for alignment in 1 2 4 8 16 32 64; do
		for policy in FF BF WF NF; do
			for index in 1 2 3 4 5 6; do
				echo "\e[33;1m**** Allocation test $$index for policy $$policy with alignment $$alignment *****\e[0m"
				make --no-print-directory -s -B tests/alloc$$index.test ALLOC_POLICY=$$policy MEM_ALIGNMENT=$$alignment 1>/dev/null 2>&1 && echo "\t\e[32mPassed!\e[0m" || exit 1
			done
			for index in 1 2; do
				echo "\e[33;1m**** Extra basic test $$index for policy $$policy with alignment $$alignment *****\e[0m"
				make --no-print-directory -s -B extests/basic_test$$index.test ALLOC_POLICY=$$policy MEM_ALIGNMENT=$$alignment 1>/dev/null 2>&1 && echo "\t\e[32mPassed!\e[0m" || exit 1
			done
			for index in 1 2; do
				echo "\e[33;1m**** Extra basic safety test $$index for policy $$policy with alignment $$alignment *****\e[0m"
				make --no-print-directory -s -B extests/safety_test$$index.test ALLOC_POLICY=$$policy MEM_ALIGNMENT=$$alignment 1>/dev/null 2>&1 && echo "\t\e[32mPassed!\e[0m" || exit 1
			done
			for index in 3 4 5 6; do
				echo "\e[33;1m**** Extra advanced safety test $$index for policy $$policy with alignment $$alignment *****\e[0m"
				make --no-print-directory -s -B extests/basic_test$$index.test ALLOC_POLICY=$$policy MEM_ALIGNMENT=$$alignment 1>/dev/null 2>&1 && exit 1 || echo "\t\e[32mPassed!\e[0m"
			done
			echo "\e[33;1m**** LS test $$index for policy $$policy with alignment $$alignment *****\e[0m"
			make --no-print-directory -s -B test_ls ALLOC_POLICY=$$policy MEM_ALIGNMENT=$$alignment 1>/dev/null 2>&1 && echo "\t\e[32mPassed!\e[0m" || exit 1
			echo "\e[33;1m**** PS test $$index for policy $$policy with alignment $$alignment *****\e[0m"
			make --no-print-directory -s -B test_ps ALLOC_POLICY=$$policy MEM_ALIGNMENT=$$alignment 1>/dev/null 2>&1 && echo "\t\e[32mPassed!\e[0m" || exit 1
		done
	done

#############################################################################

clean:
	@rm -f **.o *~ tests/*~ tests/*.out tests/*.expected *.so bin/* *.html

.PHONY: clean test mem_shell mem_shell_sim mem_alloc_test

#############################################################################

mem_shell_sim: bin/mem_shell_sim

bin/mem_shell_sim: mem_shell_sim.o mem_alloc_sim.o lib/$(LIBSIM_FILE) 
	@$(CC) $(CONFIG_FLAGS) $(CFLAGS) $(LDFLAGS) -L./lib  mem_shell_sim.o mem_alloc_sim.o -o $@ -l$(LIBSIM_NAME)

mem_shell_sim.o: mem_shell_sim.c sources/mem_alloc_types.h
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

mem_alloc_sim.o: mem_alloc_sim.c sources/mem_alloc_types.h sim_alloc.h
	@$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

#############################################################################

html: $(HTML_TARGETS)

%.html: %.md
	@pandoc $< -s -o $@

clean_html:
	@rm -f $(HTML_TARGETS)

