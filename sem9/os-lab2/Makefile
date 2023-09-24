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

# defines the set of configuration variables for the Makefile
include Makefile.config


ifeq ($(ALLOC_POLICY), BF)
$(info Using Best Fit policy)
CONFIG_FLAGS += -DBEST_FIT
else ifeq ($(ALLOC_POLICY), WF)
$(info Using Worst Fit policy)
CONFIG_FLAGS += -DWORST_FIT
else ifeq ($(ALLOC_POLICY), FF)
$(info Using First Fit policy)
CONFIG_FLAGS += -DFIRST_FIT
else ifeq ($(ALLOC_POLICY), NF)
$(info Using Next Fit policy)
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
	$(CC) $(LDFLAGS) -o $@ $^ -ldl

mem_alloc_test: bin/mem_alloc_test

bin/mem_alloc_test: mem_alloc_test.o my_mmap.o
	$(CC) $(LDFLAGS) $^ -o $@ -ldl

mem_alloc_test.o: mem_alloc.c mem_alloc_types.h my_mmap.h
	$(CC) -c -DMAIN $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

my_mmap.o: my_mmap.c my_mmap.h
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

mem_shell.o: mem_shell.c
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

#############################################################################

# Notice the presence (and the precise position in the command line) of "-ldl":
#    both are very important because mem_alloc.c uses dlsym
libmalloc.so: libmalloc.o libmalloc_std.o
	$(CC)  -shared  -Wl,-soname,$@ $^ -o $@ -ldl

libmalloc_std.o:mem_alloc_std.c mem_alloc.h mem_alloc_types.h
	$(CC) $(CONFIG_FLAGS) $(CFLAGS) -fPIC -c $< -o $@

libmalloc.o: mem_alloc-lib.o my_mmap-lib.o
	$(LD) -r $^ -o $@

mem_alloc-lib.o: mem_alloc.c mem_alloc_types.h my_mmap.h
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) -fPIC $< -o $@ -ldl

my_mmap-lib.o: my_mmap.c my_mmap.h
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) -fPIC $< -o $@

#############################################################################

test_ls: libmalloc.so
	LD_PRELOAD=./libmalloc.so ls
	LD_PRELOAD=""

test_ps: libmalloc.so
	LD_PRELOAD=./libmalloc.so ps
	LD_PRELOAD=""

#############################################################################

%.trace: %.in bin/mem_shell
	cat $< | bin/mem_shell 2>&1 

%.out: %.in bin/mem_shell
	cat $< | bin/mem_shell 2>&1 | grep -E '^ALLOC|^FREE' >$@

%.out.expected: %.in bin/mem_shell_sim
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib ; \
	cat $< | bin/mem_shell_sim ${MEM_POOL_SIZE} ${ALLOC_POLICY} ${MEM_ALIGNMENT} 2>&1 | grep -E '^ALLOC|^FREE' >$@

%.test: %.out %.out.expected
	@if diff $^  >/dev/null; then \
	  echo -e "\e[32m**** Test $@ Passed *****\e[0m"; \
	  cat $< ;\
	else \
	  echo -e "\e[31m**** Test $@ FAILED *****\e[0m"; \
	  echo -e "\t Your trace \t vs \t Expected trace";\
	  diff -y $^ ;\
	fi

#############################################################################

clean:
	rm -f *.o *~ tests/*~ tests/*.out tests/*.expected *.so bin/*

.PHONY: clean test mem_shell mem_shell_sim mem_alloc_test

#############################################################################

mem_shell_sim: bin/mem_shell_sim

bin/mem_shell_sim: mem_shell_sim.o mem_alloc_sim.o lib/$(LIBSIM_FILE) 
	$(CC) $(CONFIG_FLAGS) $(CFLAGS) $(LDFLAGS) -L./lib  mem_shell_sim.o mem_alloc_sim.o -o $@ -l$(LIBSIM_NAME)

mem_shell_sim.o: mem_shell_sim.c mem_alloc_types.h
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

mem_alloc_sim.o: mem_alloc_sim.c mem_alloc_types.h sim_alloc.h
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

#############################################################################

html: $(HTML_TARGETS)

%.html: %.md
	pandoc $< -s -o $@

clean_html:
	rm -f $(HTML_TARGETS)

