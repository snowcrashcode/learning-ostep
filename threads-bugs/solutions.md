# Solutions 

1) First let's make sure you understand how the programs generally work, and some of the key options. Study the code in `vector-deadlock.c` as well as in `main-common.c` and related files.
Now, run `./vector-deadlock -n 2 -l 1 -v` which instantiates two threads (-n 2), each of which does one vector add (-l 1), and does so in verbose mode (-v). Make sure you understand the output. How does the output change from run to run?

A: 
Program takes a source vector, and add it onto a destination vector. Note that destination vector is equal to source vector; program is just a demonstration that having mutex does not guarantee correctness of ordering.
Majority of the time, output is:

```markdown
    ->add(0,1)
    <-add(0,1)
                    ->add(0,1)
                    <-add(0,1)
```
But about 10% of the time, output looks like this:
    ```markdown
    ->add(0,1)
                    ->add(0,1) 
                    <-add(0,1)
    <-add(0,1)
```
indicating that thread B ran immediately after creation

Order of threads may be different. Ordering problem!


2) Now add the -d flag, and change the number of loops (-l) from 1 to higher numbers. What happens? Does the code (always) deadlocK?

A: 

-d flag enables threads to deadlock. Here, code might deadlock, since thread A locks `v_dst` then tries to lock `v_src` while thread B locks `v_src` and tries to lock `v_dst`.

Circular wait:
`v_dst` locked by A
`v_src` locked by B
A is waiting for `v_src` to be released (won't happen)
B is waiting for `v_dst` to be released ""

3) How does changing the number of threads (-n) change the outcome of the program? Are there any values of -n that ensures no deadlock occurs?

A:
n == 1 ensures no deadlock will occur. There will not be any concurrent access to the vectors.

For all n > 2, deadlock might occur.

4) Now examine the code in `vector-global-order.c`. First, make sure you understand what the code is trying to do; do you understand why the code avoids deadlock? Also, why is there a special case in this vector.add() routine when the source and destination vectors are the same?

A:
Code avoids deadlock by enforcing lock ordering using the lock addresses.
The if - else loop checks the address of both destination and source locks, then locks whichever lock that has the smaller address. This guarantees that both locks will always be grabbed in the same order, regardless of whichever order the locks are passed into the threads' arguments.

The special case is when somehow the destination and source lock are actually the same locks. It prevents the lock from being grabbed twice.

5) Now run the code with the following flags: -t -n 2 -l 10000 -d. How long does the code take to complete? How does the total time change when you increase the number of loops, or the number of threads?

A: 0.00s

n = 2, l = 100000 : 0.01s
n = 2, l = 1000000: 0.1s 
 
n = 10, l = 10000: 0.02s
n = 50, l = 10000: 0.1s


6) What happens if you turn on the parallelism flag (-p)? How much would you expect performance to change when each tread is working on adding different vectors (which is what -p enables) versus working on the same ones?

A: Performance should increase, since parallelism means that each thread is working on adding different vectors, thus reducing the odds of deadlocks occuring.

n = 2, l = 1000000, parallelism turned on: 0.07s

7) Now let's study `vector-try-wait.c. First, make sure you understand the code. Is the first call to `pthread_mutex_trylock()` really needed? Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as the number of threads increases?

A: Yes. The first trylock guarantees that the thread can only proceed with the vector addition (critical section) if the destination lock is acquired by the thread. If not, thread will keep retrying. Without checking, the thread might enter critical section without holding the destionation lock, leading to incosistent locking order between threads, increasing chance of deadlock.

Number of retries increases (exponentially) as number of threads increases.
A lot slower than global order, presumably due to the repeated retries.

8) Now let's look at `vector-avoid-hold-and-wait.c`. What is the main problem with this approach? How does its performance compare to the other versions, when running both with -p and without it?

A: Everytime a thread runs the program, it has to acquire and release the global lock. This results in high locking overhead, reduing efficiency of program.

Also, only one thread is allowed to add at a time.

-p increases performance of this program by a bit.

Slightly slower compared to other programs.


9) Finally, let's look at `vector-nolock.c`. This version doesn't use locks at all; does it provide the exact same semantics as the other version? Why or why not?

A:
Yes.

`asm volatile`: Tells compiler to insert assembly code directly into program without optimising it away or moving it, ensuring that it executes exactly where it is placed.

`xaddl`: Exchange and add instruction. Adds a variable and return the original value. Atomic operation.

`lock`: Ensure instruction is atomic on multi-core systems


10) Now compare its performance to the other versions, both when threads are working on the same two vectors (no -p) and when each thread is working on separate vectors (-p). How does this no-lock version perform?

A: Without parallelism: Much slower. With parallelism: Faster, but slightly slower than other versions