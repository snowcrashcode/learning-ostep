# Overview

Measure the size and cost of accessing a TLB.

Access some number of pages within an array and time those accesses. For example, let's say the TLB size of a machine happen to be 4.
If you write a program that touches 4 or fewer pages, each access should be a TLB hit, and thus relatively fast.
However, once you touch 5 pages or more, repeatedly in a loop, each access will suddenly jump in cost, to that of a TLB miss.

Basic code to loop through an array:

``` C
int jump = PAGESIZE / sizeof(int);
for (int i = 0; i < NUMPAGES * jump; i += jump )
    a[i] += 1;
``` 
In this loop, one integer per page of the array a is updated, up to the number of pages specified by NUMPAGES.
By timing such a loop repeatedly (say, a few hundred million times in another loop around this one, or however many loops are needed to run for a few seconds), you can time roughly how long each access takes on average. 
By looking for jumps in cost as NUMPAGES increases, you can roughly determine how big the first-level TLB is, determine whether a second-level TLB is (and how big is it if it does), and in general get a good sense of how TLB hits and misses can affect performances.

NOTE: I think this is under the assumption/fact that TLB can have at most two levels.

## Questions

1) How precise is a timer like gettimeofday()? How long does an operation have to take in order for you to time it precisely? (This will help determine how many times, in a loop, you'll have to repeat a page access in order to im time it successfully.)
- CLOCK_PROCESS_CPUTIME_ID has a resolution of 1 nanosecond on Linux, 1000 nanoseconds on MacOS.
- General guideline is **the operation should be at least 100 times longer than the timer's resolution** to provide a buffer to ensure that the noise and measurement overhead don't dominate the measurement

2) Write a program called tlb.c that can roughly measure the cost of accessing each page. Inputs to the program should be: the number of pages to touch and the number of trials.
- My computer does have a 2-level TLB architecture.
    - Initial rise -> L1 TLB, can store 16 pages
    - Plateau -> L2 TLB, can store about 4096 pages
    - Decrease after 4096 pages -> May indicate that pages are spilling over to a higher level cache (L3 or main memory)

3) Write a script in bash to run this program, while varying the number of pages accessed from 1 up to a few thousand, perhaps incrementing by a factor of two per iteration.
Run the script on different machines and gather some data. How many trials are needed to get reliable measurements?

4) Next, graph the results. Use a good tool like ploticus or even zplot. Visualization makes the data much easier to digest; why do you think that is?
- Easier to see the distinct levels of TLB

5) One thing to watch out for is compiler optimizations. Compilers do all sorts of clever things, including removing loops which increment values that no other part of the program subsequently uses. How can you ensure that the ocmpiler does not remove the main loop above from your TLB size estimator?
-O0 to disable optimization when compiling in the Makefile.

6) Another thing to watch out for is the fact that most systems today ship with multiple CPUs, and each CPU has its own TLB hierachy. To really get good measurements, you have to run your code on just one CPU, instead of letting the scheduler bounce it from one CPU to the next. How can you do that? Look up "pinning a thread". What will happen if you don't do this, and the code moves from one CPU to another?
- use sched_setaffinity(2)

7) Another issue that might arise relates to initialization. If you don't initialize the array a above before accessing it, the first time you access it will be very expensive, due to initial access costs such as demand zeroing. Will this affect your code and its timing? What can you do to counterbalance these potential costs?
- Use calloc() to initialize array then measure the time. If not, the timings will be a lot larger due to having to initialize (i.e., access the array for the first time) when timing.