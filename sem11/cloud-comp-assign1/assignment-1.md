# COMP4651 Assignment 01: EC2 Measurement (8 marks)

### Deadline: 23:59, Oct. 10, Thursday
### Finished: ??:??, Oct. 9, Wednesday

---

### Name: Aleksandr Sergeev
### Student Id: `21158862`

### Email: asergeev@connect.ust.hk

---

## Question 1: Measure the EC2 CPU and Memory performance

1. (0.5 mark) Report the name of measurement tool used in your measurements (you are free to choose *any* open source measurement software as long as it can measure CPU and memory performance). Please describe your configuration of the measurement tool, and explain why you set such a value for each parameter. Explain what the values obtained from measurement results represent (e.g., the value of your measurement result can be the execution time for a scientific computing task, a score given by the measurement tools or something else).

    > I have chosen `sysbench` tool to benchmark **CPU and memory performance**.
    > It can be installed with only one simple command: `sudo apt-get install -y sysbench`.
    > All the tests of `sysbench` accept several parameters for benchmark limitation, i.e. requests number, stack size, time, etc.
    > I will set them to unlimited value (unless any problems will occur).
    > For the test thread number (`threads` parameter), I set it equal to 2, so that there would be instances with more and less vCPUs than the number of processes.
    > *CPU benchmarking* is performed by calculation of prime numbers, up until `cpu-max-prime` number.
    > This number is set to one million: it is large enough, but still doesn't take too much time to compute.
    > The most interesting performance metric is CPU speed, that is measured in events (the prime number calculations) per second (event time is the sum of times taken by all working threads).
    > *Memory benchmarking* is performed by allocating memory buffer of size `memory-block-size` and performing either read or write operation (`memory-oper`) in it again and again until `memory-total-size` is covered.
    > Buffer size is set to 512K so that it doesn't impact the tiniest VMs performance, total size is set to 100G to perform operation with buffer sufficient amount of times.
    > Operation is set to `write`, because write operation is always considered to be slower and more resource-consuming.
    > As for the performance metric, I was using data transferring speed for comparison, that is number of megabytes that could be written per second.
    > I have chosen `iperf` and `ping` tools to benchmark **network performance**.
    > It can be installed with only one simple command: `sudo apt-get install -y iperf iputils-ping`.
    > *Throughput benchmarking* is measured by sending data packets between two hosts, several parameters of the `iperf` tool are adjustable, but I preferred to leave the defaults.
    > The only parameter that was changed is TCP window size, it was set to 64K, that is the default TCP window size (according to [Microsoft docs](https://learn.microsoft.com/en-us/troubleshoot/windows-server/networking/description-tcp-features)).
    > *Round trip time benchmarking* is measured by calculating time delta between sending special SMTP request packets and retrieving responses.
    > The only adjusted parameter was the sending request `count`, it was changed from unlimited to 32 for automatization sake.

2. (1 mark) Run your measurement tool on general purpose `t2.small`, `t3.medium`, and `c3.large` Linux instances, respectively, and find the performance differences among these instances. Launch all the instances in the **N. Virginia** region. Does the performance of EC2 instances increase commensurate with the increase of the number of vCPUs and memory resource?

    In order to answer this question, you need to complete the following table by filling out blanks with the measurement results corresponding to each instance type.

    > Region: `N. Virginia`. Use `Ubuntu Server 24.04 LTS (HVM)` as AMI.

    | Size        | CPU performance | Memory performance |
    | ----------- | --------------- | ------------------ |
    | `t2.small`  |      5.02       |      18529.14      |
    | `t3.medium` |      2.52       |      15079.82      |
    | `c3.large`  |      2.77       |      16528.27      |

    > According to the [AWS instance type description](https://aws.amazon.com/ec2/instance-types/) I would have expect gradual increase of performance.
    > However I found out that `t2.small` instances were significantly faster both in terms of computing and memory access (I doublechecked that 3 times, you can find all the benchmarking reports uploaded).
    > That looks weird because according to the documentation (and common sense) they should be slower, at least because of thread context switching.
    > The only possible explanation that I can think of is that `t2.small` instances are specifically boosted when running `sysperf`.
    > The CPU and memory performance growth from `t3.medium` to `c3.large` is expected.

## Question 2: Measure the EC2 Network performance

1. (2 mark) The metrics of network performance include **TCP bandwidth** and **round-trip time (RTT)**. Within the same region, what network performance is experienced between instances of the same type and different types? In order to answer this question, you need to complete the following table.

    > Region: `N. Virginia`. Use `Ubuntu Server 20.04 LTS (HVM)` as AMI. You should launch **6** instances in total.

    | Type                      | TCP b/w (Mbps) | RTT (ms) |
    | ------------------------- | -------------- | -------- |
    | `t2.small` - `t2.small`   |       872      |  0.982   |
    | `t3.medium` - `t3.medium` |      1390      |  0.289   |
    | `c3.large` - `c3.large`   |      1190      |  0.207   |
    | `t2.small` - `c3.large`   |       922      |  0.776   |
    | `t3.medium` - `c3.large`  |      1120      |  0.246   |
    | `t3.medium` - `t2.small`  |       814      |  0.747   |

    > Within the same region TCP bandwidth is high and RTT is small between all instances.
    > That happens because packets do not even have to leave the datacenter high-performance internal network.
    > At the same time, we can notice that network performance improves from `t2.small` to `c3.large`, that might mean that more expensive instances are provided with more efficient network links.

2. (1 mark) What about the network performance for instances deployed in different regions? In order to answer this question, you need to complete the following table.

    > Region: `N. Virginia`/`Oregon`. Use `Ubuntu Server 20.04 LTS (HVM)` as AMI. All instances are `t3.medium`.

    | Connection                | TCP b/w (Mbps) | RTT (ms) |
    | ------------------------- | -------------- | -------- |
    | N. Virginia - Oregon      |       7.12     |  63.264  |
    | N. Virginia - N. Virginia |        838     |   0.405  |
    | Oregon - Oregon           |        926     |   0.372  |

    > Just as expected, RTT between different datacenters (and different parts of the country) is larger and bandwidth is smaller.
    > Network performance within the datacenters is significantly higher, but still different.
    > My guess is that `N. Virginia` is the default location offered to users, and so it might have higher workload thab `Oregon`.

3. (1 mark) Is network performance consistent over time? You can do measurements at different times of a day and compare the results. Please give at least 2 possible reasons why network performance is inconsistent.

    > Network performance is not consistent over time, still it does not change a lot.
    > It is always high inside datacenters, it is always low between datacenters.
    > The reasons for that might be both internal (during daytime datacenter has more workload) and external (again, during daytime network between datacenters can be congested with high amount of traffic).
    > Moreover, there could be some events that impact performance greatly (datacenter DDoS attacs, service maintenance hours, network outages, etc.) but I have not experienced any.

## Question 3：Decision Trees

We would like to construct a decision tree for $n$ vectors each with $m$ attributes.

1. (0.5 mark) Assume that there exists $i$ and $j$ such that for ALL vectors $X$ in our training data, these attributes are equal ($x_i=x_j$ for all vectors where $x_i$ is the $i$’th entry in the vector $X$). Assume that we break ties between them by using $x_i$ (that is, if both lead to the same conditional entropy we would use $x_i$). Can removing attribute $j$ from our training data change the decision tree we learn for this dataset? Explain briefly.

    > No, it can not change the decision tree.
    > If $x_i=x_j$ for all the vectors in the dataset, then attributes $i$ and $j$ provide the same information.
    > Given that $x_j$ can always be replaced with $x_i$, removing $x_j$ would not affect tree building.

2. (0.5 mark) Assume we have two equal vectors $X$ and $Z$ in our training set (that is, all attributes of $X$ and $Z$ including the labels are exactly the same). Can removing $Z$ from our training data change the decision tree we learn for this dataset? Explain briefly.

    > Removing vector $Z$ can only change the decision tree, if probabilistic methods are used for creation of splits.
    > Removing $Z$ in that case would decrease the frequency of vector $X$, probably leading to some splitting differences.
    > However, that impact should not be very significant in case of big datasets or when all the tree splits are deterministic.

For the next set of questions consider a dataset with continuous attributes. For such attributes we can use threshold splits to determine the best partition for a set of vectors. Assume we are at the root and we have $n$ vectors, all with different (continuous) values for attribute $x_1$.

3. (0.5 mark) Assume we would like to use binary splits. For such splits we need to choose a value $a$ and split the data by propagating all vectors with $x_1<a$ to the left and those with $x_1>=a$ to the right. For any value of $a$ we consider we would like to have at least one vector assigned to each of the two branches of the split. How many values of a do we need to consider?

    > If every split needs to have at least one vector in both branches, we could consider values between every consecutive pair of $x_1$.
    > If there are $n$ different values of $x_1$, we will have to consider $n-1$ potential splits.

4. (0.5 mark) Assume we would like to use three way splits. For such splits we need to chose values $a$ and $b$ such that $a$ < $b$ and split the data into three sets: $x_1 < a, a <= x_1 < b,
x_1 >= b$. Again we require that for any value of $a$ and $b$ that we consider at least one vector would be assigned to each of the three branches. How many $\{a,b\}$ do we need to consider?

    > Just as before, we will consider values between consecutive pairs of $x_1$ as potential splits.
    > Since value $a$ should always be less then value $b$, we will choose $a$ from first $n-2$ pairs of $x_i$ and $b$ from all the pairs after.
    > The result is $\sum_{i=1}^{n-2} (n-1-i) = \frac{(n-1)(n-2)}{2}$.

5. (0.5 mark) For a given three way split at the root (parameterized by an $\{a,b\}$ pair) can we reconstruct the same split with a tree that uses only binary splits?
If no briefly explain why.
If yes, show the tree that leads to the same set of leaves with the same nodes in each leaf as the three way split.

    > We can reconstruct it by splitting root at $a$ and then splitting the "greater" leaf at $b$.
    >
    > ```txt
    >             Root
    >           /      \
    >       x1 < a      \
    >        /           \
    >      ...         x1 >= a
    >                   /    \
    >              x1 < b    x1 >= b
    >                /          \
    >              ...          ...
    > ```
