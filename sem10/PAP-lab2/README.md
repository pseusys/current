# PAP-2

## Run with

1. bubble:
   ```shell
   make bubble TIMES=15
   ```
2. mergesort:
   ```shell
   make mergesort TIMES=15
   ```
3. odd-even
   ```shell
   make odd-even TIMES=15
   ```
4. quicksort
   ```shell
   make quicksort TIMES=15
   ```


## TODOs

1. `bubble`: study pragma omp in order to find the best solution for scheduling (or smth else)
2. `mergesort`: study pragma omp in order to find solution to minimize task number
3. `even-odd`: analyze parallelization perspectives, compare with `bubble`
4. `quicksort`: improve parallel performance, find omp means for that
5. write a report
