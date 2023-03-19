# PAP-2

## Run with

1. bubble:
   ```shell
   make bubble TIMES=8 THREADS=4
   ```
2. mergesort:
   ```shell
   make mergesort TIMES=15 THREADS=4
   ```
3. odd-even
   ```shell
   make odd-even TIMES=8 THREADS=4
   ```
4. quicksort
   ```shell
   make quicksort TIMES=15 THREADS=4
   ```

## Report
Alexander Sergeev and Pia Döpper

We adapted the makefile for more handy using. You can find in the beginning of this README, the precise instructions to compile and run the different algorithms.

The algorithms are run on an laptop with ubuntu 20.04 Os (64bit), Intel® Core™ i5-6300U CPU @ 2.40GHz × 4 processer, memory 7,2 GiB. 

### Exercise 3 `Bubblesort`
We have implemented the Bubblesort Algorithm following the given pseudocode. We used the static scheduling policy, because our chunks have the same size and so we do not need the communicating/synchronizing part and we are only using the static policy. The code can be find in sources/bubble.c. We can run the bubblesort algortihm only for  Matrices until 2^8 sizes(Laptop limits). 

We have conducted the following Graphs: 

TODO

### Exercise 4 `Mergesort`
We have implemented the mergesort algortihm with openmp task. Like this we can ensure that the sorting of the half of the matrices are done before we will merge them together. The sorting of the two parts of the array can be done in parallel, but the merging should start after. In order to optimize the merge sort and reduce the task number, we ware checking if the array is already sorted and if yes, we are reducing the tasks. The analyze shows that it leads to an small improvement in execution time.

We have conducted the following Graphs: 

TODO

### Exercise 5 `Odd even Sort`
We have implemented the even-odd algorithm in a sequential way and a parallelized way. 
The even-odd algorithm is more suitable for parallization than the bubblesort algorithm because it has two sort strings that can be parallized. The array is sorted in two intervals, with even and odd indexes respectively. We can parallelize this and get better performance. 

We have conducted the following Graphs: 

### Exercise 7 `Quicksort`
We have implemented the Ouicksort parallel version. The parallel version is always slower than the sequential one and we have no clou why we do not found any improvement. We followed the hint in dividing the array into chunks, sort them and then merge them together. Like the mergesort algorithm we have used the openmp task syntax to ensure the different subtasks in the algortihm.


