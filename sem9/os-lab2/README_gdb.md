# Using gdb for programs with arguments / redirections / environment variables

In this lab (or others), you may have to use the `gdb` debugger with programs that must be launched with:

* one or several arguments
* and/or input redirections (standard input, standard output, standard error)
* and/or the definition of specific environment variables (for example, `LD_PRELOAD`)

To do this, there are several features of `gdb` than you can choose from.
One of the simplest ways is to do it is to use several steps:

* First, launch `gdb ./myprog`
* and then use the `run` command in the `gdb` shell to provide the required arguments/redirections  

Another possibility is to proceed as follows:

* First, launch `gdb ./myprog`
* Then use (if necessary) the `set args` command to set up the argument(s) and redirections. For example: `set args arg1 arg2 < ./input.txt`
* Then use (if necessary) the `set environment` to set up the environment variable(s). For example: `set environment LD_PRELOAD ./mylib.so`
* Then start the execution with: `run`
	
For concrete examples, you can look at the following discussions:

* [How to use gdb with input redirection?](https://stackoverflow.com/questions/4758175/how-to-use-gdb-with-input-redirection/13297354) (from StackOverflow)
* [How to pass arguments and redirect stdin from a file to program run in gdb?](https://stackoverflow.com/questions/4521015/how-to-pass-arguments-and-redirect-stdin-from-a-file-to-program-run-in-gdb) (from StackOverflow)
* [How to use gdb with LD_PRELOAD](https://stackoverflow.com/questions/10448254/how-to-use-gdb-with-ld-preload) (from StackOverflow)


