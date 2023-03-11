# PAP-2

## Run with

1. bubble:
   ```shell
   make clean && make && ./bubble.run 15
   ```
2. mergesort:
   ```shell
   make clean && make && ./mergesort.run 15
   ```
3. even-odd
   ```shell
   make clean && make && ./even-odd.run 15
   ```
4. quicksort
   ```shell
   make clean && make && ./quicksort.run 15
   ```


## TODOs

1. Make Makefile more handy: autoremove, wildcard lists, builds in build folder, rules for each algo
2. `bubble`: study pragma omp in order to find the best solution for scheduling (or smth else)
3. `mergesort`: study pragma omp in order to find solution to minimize task number
4. `even-odd`: analyze parallelization perspectives, compare with `bubble`
5. `quicksort`: improve parallel performance, find omp means for that
6. write a report
