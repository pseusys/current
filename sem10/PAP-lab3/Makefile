.ONESHELL:
.DEFAULT_GOAL := run

PROC=`nproc --all`
PROG=1_hello_world

ARGS=16



build/%.run: sources/%.c sources/utils.c sources/utils.h
	@ # Compile and run MPI source file.
	mkdir -p build
	rm -f $@ 2> /dev/null
	mpicc -o $@ $^ -lm

run: build/$(PROG).run
	@ # Compile and run MPI program.
	mpirun -n $(PROC) --oversubscribe $^ $(ARGS)
.PHONY: run



hello_world:
	@ # Compile and run hello world program.
	make run PROG=1_$@
.PHONY: hello_world

odd_even:
	@ # Compile and run even odd hello world program.
	make run PROG=2_$@
.PHONY: odd_even

simple:
	@ # Compile and run simple communication program.
	make run PROC=2 PROG=3_$@
.PHONY: simple

ping_pong:
	@ # Compile and run ping pong program.
	make run PROC=2 PROG=4_$@
.PHONY: ping_pong

ring:
	@ # Compile and run ring communication program.
	make run PROC=8 PROG=5_$@
.PHONY: ring

non_blocking:
	@ # Compile and run non blocking communication program.
	make run PROC=2 PROG=6_$@
.PHONY: non_blocking

broadcast:
	@ # Compile and run broadcasting program.
	make run PROG=7_$@
.PHONY: broadcast

sum_reduction:
	@ # Compile and run program that calculates process id sum using reduction.
	make run PROG=8_$@
.PHONY: sum_reduction

global_reduction:
	@ # Compile and run program that calculates process id sum using global reduction.
	make run PROG=9_$@
.PHONY: global_reduction

wait_reduction:
	@ # Compile and run program that calculates process id sum using wait all.
	make run PROG=10_$@
.PHONY: wait_reduction

barrier:
	@ # Compile and run program that implements synchronization with barrier.
	make run PROC=4 PROG=11_$@
.PHONY: barrier

pi_calc:
	@ # Compile and run program that implements calculation of PI number using ARGS intervals.
	make run PROG=12_$@
.PHONY: pi_calc

virtual_grid:
	@ # Compile and run program that orders processes into a virtual grid.
	make run PROG=13_$@
.PHONY: virtual_grid

square_grid:
	@ # Compile and run program that orders processes into a square virtual grid.
	make run PROG=14_$@
.PHONY: square_grid

comm_split:
	@ # Compile and run program that creates row and column communicators for a process square virtual grid.
	make run PROG=15_$@
.PHONY: comm_split

token_passing:
	@ # Compile and run program that passes a token along a square virtual grid.
	make run PROG=16_$@
.PHONY: token_passing

row_broadcast:
	@ # Compile and run program that performs a broadcast along rows in a square virtual grid.
	make run PROG=17_$@
.PHONY: row_broadcast

column_sum:
	@ # Compile and run program that calculates sum along columns in a square virtual grid.
	make run PROG=18_$@
.PHONY: column_sum

grid_shifts:
	@ # Compile and run program that performs shifts in a square virtual grid.
	make run PROG=19_$@
.PHONY: grid_shifts
