.ONESHELL:
.DEFAULT_GOAL := run

PROC=`nproc --all`
PROG=1_hello_world



build/%.run: sources/%.c sources/utils.c sources/utils.h
	@ # Compile and run MPI source file.
	mkdir -p build
	rm -f $@ 2> /dev/null
	mpicc -o $@ $^

run: build/$(PROG).run
	@ # Compile and run MPI program.
	mpirun -n $(PROC) --oversubscribe $^
.PHONY: run



hello_world:
	@ # Compile and run hello world program.
	make run PROG=1_hello_world
.PHONY: hello_world

odd_even:
	@ # Compile and run even odd hello world program.
	make run PROG=2_odd_even
.PHONY: odd_even

simple:
	@ # Compile and run simple communication program.
	make run PROC=2 PROG=3_simple
.PHONY: simple

ping_pong:
	@ # Compile and run ping pong program.
	make run PROC=2 PROG=4_ping_pong
.PHONY: ping_pong

ring:
	@ # Compile and run ring communication program.
	make run PROC=8 PROG=5_ring
.PHONY: ring

non_blocking:
	@ # Compile and run non blocking communication program.
	make run PROC=2 PROG=6_non_blocking
.PHONY: non_blocking

broadcast:
	@ # Compile and run broadcasting program.
	make run PROG=7_broadcast
.PHONY: broadcast

sum_reduction:
	@ # Compile and run program that calculates process id sum using reduction.
	make run PROG=8_sum_reduction
.PHONY: sum_reduction

global_reduction:
	@ # Compile and run program that calculates process id sum using global reduction.
	make run PROG=9_global_reduction
.PHONY: global_reduction

wait_reduction:
	@ # Compile and run program that calculates process id sum using wait all.
	make run PROG=10_wait_reduction
.PHONY: wait_reduction

barrier:
	@ # Compile and run program that implements synchronization with barrier.
	make run PROC=4 PROG=11_barrier
.PHONY: barrier
