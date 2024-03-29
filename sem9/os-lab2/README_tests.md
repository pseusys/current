---
title: README_tests
---

## Introduction

The files `allocX.in`, included in the directory **`tests`**, are examples
of inputs for the program `mem_shell`. Do not hesitate to write your
own scenarios. A short description of each of these files is given
below.

The program `mem_shell_sim` generated by the Makefile and stored
in the directory `./bin` generates the expected trace for a given
input and thus allows you to check the output of the `mem_shell`
that runs a test scenario using your memory allocator.

Note that the program `mem_shell` can also be run interactively
with:
```
    bin/mem_shell
```
then type `h` to get some help.

## Syntax

The syntax of `.in` files is simple. The only rule is to write 1
command per line. Four types of commands are available:
 
 * `aXX`: asks for the allocation of a block of size XX.
 * `fY`: asks to free the block allocated by the Y-th call to malloc
 * `p`: prints the state of the memory
 * `q`: tells the program to terminate

## Running a test

To test if a scenario works as expected, use the provided Makefile and
simply run (for the case of `alloc1.in`):
```
make -B tests/alloc1.test
```

To obtain the output generated when executing a test with your 
implementation, replace the *.test* suffix with *.trace*, as follows:
```
make -B tests/alloc1.trace
```

## Configuring a test

Note that the file `Makefile.config` includes the definition of the
main parameters of the allocator. You can change the value of these
parameters to test your allocator under different conditions.

Note that while running a test, you can overwrite some of the
parameters defined in the configuration file using the following
command:
```
make -B ALLOC_POLICY=NF tests/alloc1.test
```

In this example, we force the test to be executed with the *Next Fit*
policy.



## Specification of the expected output

The program `mem_shell_sim` makes a few assumptions when generating
the trace for a test case. **You have to follow the same rules in your
implementation for it to be considered as correct**.
    
  * Calling `memory_alloc()` with a size of 0 returns a valid pointer that can later be freed.
  * If `memory_alloc()` does not manage to allocate a block, the program terminates
    with a call to `exit(0)`. (Note that this is not the standard behavior of the 
    `malloc()` function.)
  * Trace functions are called appropriately.
  * The list of free blocks is ordered according to increasing memory
    addresses.
  * With the *Next fit* policy, if a block immediately preceding the
    block to be used as a starting point for the search during the next allocation, is freed,
    the two blocks are coalesced and the starting block for the next
    search becomes the new coalesced block.

*If you followed all these rules, and `mem_shell_sim` generates a
different trace from the one generated by your allocator, but you are
sure your allocator is correct, please do not hesitate to send an email
to the teaching staff with information about your problematic
scenario. There can always be errors in the simulation.*


## Description of the provided scenarios

Note : For the tests described below (`allocX.in`), we advise you to use the default size for the heap (1024 bytes) - see `Makefile.config`. However, you can of course use a different (larger) size for your own additional tests.

### alloc1.in: 

A simple sequence of memory allocations.

### alloc2.in:

A sequence of allocations followed by some free calls.

### alloc3.in:

A sequence of interleaved allocation and free calls that may behave
differently depending on the allocation policy.

### alloc4.in:

A sequence of interleaved allocation and free calls that imply
coalescing adjacent free spaces.

### alloc5.in

A complex sequence of allocation and free calls.

### alloc6.in

A complex sequence of allocation and free calls.

## A few more tests

The provided Makefile also allows you to test whether your memory
allocator works with standard programs. The tests that can be run are:

```
  make test_ls
  make test_ps
```
