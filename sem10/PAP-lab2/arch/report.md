# PAP Lab 2 
## Alexander Sergeev and Pia Döpper

The repository can be found [here](https://github.com/pseusys/current/tree/master/sem10/PAP-lab2)

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

### Exercise 3 `Bubblesort`
We have implemented the Bubblesort Algorithm following the given pseudocode. We used the static scheduling policy, because our chunks have the same size and so we do not need the communicating/synchronizing part and we are only using the static policy. The code can be find in `sources/bubble.c`.

#### Parallel bubblesort algorithm performance
Here we see an Graph for the speedups. The size of the Array is 2^8. We can see that the execution time is growing with more threads we are using. The problem size is the amount of threads.

![](https://i.imgur.com/2mgUold.jpg)



#### Parallel bubblesort algorithm performance
Here we have like an graph with for the parallel bubblesort with 4 threads and the size of the array as problem size. The number indicates the exponent x for the array of size 2^x.

![](https://i.imgur.com/yk1rpRz.jpg)


We can see that the bubblesort algorithm in the parallel version has a really bad perfomance. 

#### Optimized parallel bubblesort algorithm performance

The idee of this algorithm is, instaed of only changing the corner of the parallel sorted parts, we will merge them together. This lead to an better performance.

![](https://i.imgur.com/TDzIHpt.jpg)

![](https://i.imgur.com/CSmgbXv.jpg)



### Exercise 4 `Mergesort`
We have implemented the mergesort algortihm with openmp task. Like this we can ensure that the sorting of the half of the matrices are done before we will merge them together. The sorting of the two parts of the array can be done in parallel, but the merging should start after. In order to optimize the merge sort and reduce the task number, we are checking if the array is already sorted and if yes, we are reducing the tasks. The analyze shows that it leads to an small improvement in execution time.`sources/mergesort.c`.

#### Sequential mergesort algorithm performance
The following Graph shows the exponential growing of the execution time for the sequential mergesort algorithm with problemsize 2^X. 

![](https://i.imgur.com/siExATr.jpg)

#### Parallel mergesort algorithm performance
For the parallel mergesort and an array size of 2^22 we get this graph. It show a good speedup performance of the graph. The problemsize is the number of threads. With growing numer of threads the execution time becomes smaller.


![](https://i.imgur.com/hjz0tWV.jpg)


#### Optimized mergesort algorithm performance
As an optimization, we added a check to merge sort algorithm, so the merge function wasn't executeed for already sorted arrays.
For the same configurations of our optimized mergesort we even get better performanse result as you can see here:


![](https://i.imgur.com/sBlGBey.jpg)



### Exercise 5 `Odd even Sort`
We have implemented the even-odd algorithm in a sequential way and a parallelized way. 
The even-odd algorithm is more suitable for parallization than the bubblesort algorithm because it has two sort strings that can be parallized. The array is sorted in two intervals, with even and odd indexes respectively. We can parallelize this and get better performance. We see a big difference in improvement between this algorithm an the bubblesort algorithm. The code can be found in `sources/odd-even.c`.

![](https://i.imgur.com/RExZ27l.jpg)

As for the parallel bubblesort algorithm we added an analysis for the parallel bubblesort with 4 threads and as problemsize the array size. We see here improvements to, compared to the bubblesort. But still compared to the sequential results it has a bad performance. The sequential odd-even algorithm need likes the sequential bubblesort only maximal 0.001 sec for the execution. 

![](https://i.imgur.com/vTlJUaQ.jpg)


![](https://i.imgur.com/GEUANyJ.jpg)


![](https://i.imgur.com/2bDmzYw.jpg)




### Exercise 7 `Quicksort`
We have implemented the Ouicksort parallel version. The parallel version is always slower than the sequential one and we have no clou why we do not found any improvement. We followed the hint in dividing the array into chunks, sort them and then merge them together. Like the mergesort algorithm we have used the openmp task syntax to ensure the different subtasks in the algortihm. The code can be found in `sources/quicksort.c`.

The execution time for the sequential quicksort algorithm for an array size of 2^14 is 0.121 and smaller than every execution time of the parallel quicksort algortihm. 

![](https://i.imgur.com/OvhH4g5.jpg)


