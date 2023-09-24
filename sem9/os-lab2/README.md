---
title: README
---


This is the README file for the memory allocator project of the OS course in M1 Mosig.

## About tests

Please read [README_tests](./README_tests.html) for a description of the provided tests and
how to run them.


## Compiling the code

A Makefile is provided to automatically build the memory allocator and
run tests.

### Basic commands

To run a test, simply use the following command:
```
    make -B tests/alloc1.test
```

To simply build the interactive shell, run:
```
    make -B mem_shell
```

**Note**: It is important to use option `-B` when running `make` to
force all files to be recompiled and be sure that all your changes have
been taken into account.

**Note 2**: You can also run the test defined in the main function of the
file mem_alloc.c by running:
```
    make -B mem_alloc_test
```

Please read the Makefile directly for more information.

### Configuring properties

The file *Makefile.config* defines the main properties of the memory
allocator. You can modify this file to change the properties of your
allocator (for instance, the chosen placement strategy like first fit or best fit). Do not forget to recompile your code after any modification
made to this file.

Note that a property can also be changed from the command line when
calling `make` (without editing the Makefile or Makefile.config files). Here is an example that selects the *next fit* strategy:
```
    make -B ALLOC_POLICY=NF mem_shell
```

### Using `gdb` for debugging

Please read [gdb_README](./gdb_README.html) for instruction on how to run your code with `gdb`.

**We strongly advise using gdb to facilitate the debugging of your allocator.**

***

## List of provided files

  * *mem_alloc.h*: The interface of your allocator.
  
  * *mem_alloc_types.h*: The data types used by the allocator.
  
  * *mem_alloc.c*: The code of your allocator.
    
  * *my_mmap.h* and *my_mmap.c*: Wrapper code for simplifying the usage of mmap.
  
  * *mem_alloc_std.c*: Re-implements default allocation (malloc, free, ...) so that existing programs can be run with your allocator.
  
  * *mem_shell.c*: a simple program to test your allocator.
  
  * *lib/libsim.so*: Library used for the generation of the expected trace for a scenario (compiled for Linux on Intel x86_64)
  
  * *sim_alloc.h*: the interface of the libsim library
  
  * *mem_alloc_sim.h* and *mem_alloc_sim.c*: The interface of the simulator
  
  * *mem_shell_sim.c*: mem_shell implemented using the simulator
 
  * *tests/allocX.in*: Test scenarios
  
  * *Makefile*: Rules to build the project
  
  * *Makefile.config*: Definition of the (default) parameters of the memory
    allocator
 
  * *README.md*: this file
 
  * *README_tests.md*: Readme file about the tests
  
  * *gdb_README.md*: Readme file about gdb
