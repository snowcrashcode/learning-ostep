# Solutions
 
1) Examine flag.s. This code "implements" locking with a single memory flag. Can you understand the assembly?

A: Yes. It is an implementation of a spinlock to manage access to a critical section.

.acquire
1) currrent value of lock flag is moved into %ax register
2) Check if value inside %ax (flag) is zero. This is done by peforming an AND operation between %ax and 0, setting zero flag if `%ax` is zero.  
3) If zero flag is not set, indicates that lock is acquired; program will jump back to .acquire label and try again (spinlock behaviour)
4) If zero flag was set, means the lock is free, so program stores 1 into flag variable, indicating that the lock is now acquired (by the thread that called this process)

Critical Section
1) Current value of count is moved into %ax register
2) Value of %ax is incremented by 1
3) Incremented value is stored back into the `count` variable

Release Lock
1) Lock is released by setting flag to 0

Looping Check 
1) Value inside %bx is decremented by 1
2) `test` instruction checks if `%bx` is now zero. 
3) if %bx is greater than 0, prrogram jumps to .top label, restarting the loop

Halt: Program halts execution


2) When you run with the defaults, does flag.s work? Use the -M and -R flags to trace variables and registerrs (and turn on -c to see their values). Can you predict what value will end up in flag?

A: `./x86.py -p flag.s -M flag,count -R ax,bx -c`. Final value in flag is 0. 

Process 1003 moves the value 1 into the flag. Process 1007 moves the value 0 into the flag. Hence, it is expected that the final value of flag is 0.



3) Change the value of the register %bx with the -a flag (e.g., -a bx=2, bx=2 if you aer running just two threads). What does the code do? How does it change your answer to the question above?

A: Process 1009 tests the value inside %bx against the value 0. Next, process 1010 (since value of %bx is 1, which is greater than 0) jumps to .top, repeating from process 1000, this time with %bx set to 1 instead of 2.

Note that flag value remains unchanged at 0. This is the flag value is reset back to 0 right before the test of %bx is done.

Thread 0 will run two loops -> first loop decrements %bx from 2 to 1, then second loop decrements %bx from 1 to 0, which causes process 1010 to not execute, hence halting thread 0 and switching to thread 1. Since %bx is already 0, when process 1009 is executed to test %bx, value of %bx is -1 which is not greater than 0, hence thread 1 is halted after only 1 loop.
 
4) Set bx to a high value for reach thread, and then use the -i flag to genearte different interrrupt frequencies; what values lead to a bad outcomes? Which lead to good outcomes?

A: `python3 x86.py -p flag.s -M flag,count -R ax,bx -c -a bx=10,bx=10 -i 'interrpt_frequency'
Good interrupt frequency (value of count and %ax able to increment to 20) = 11, 15, 16, >= 21
Bad interrupt frequencyu: All integer values not included in good interrupt frequency


5) Now let's look at the program test-and-set.s. First, try to understand the code, which uses the xchg instruction to build a simple locking prirmitive. How is the lock acquire written? How about lock release?

A:
<u>Lock Acquire</u>
1) Move value of 1 into %ax register (mov)
2) Perform atomic swap (exchange) of value inside mutex variable and %ax register 
3) Test 0 and %ax together. If %ax == 0 (i.e., mutex was previously 0, free), then lock is free.
4) If %ax != 0, jump back to .acquire and retry

<u>Lock Release</u>
Move value of 0 into mutex variable, freeing the lock.


6) Now rurn the code, changing the value of the interrupt interval (-i) again, and making sure to loop for a number of times. Does the code always work as expected? Does it sometimes lead to an inefficient use of the CPU? How could you quantify that?

A: Mutex is 0, so it worked as expected.

**Spinlock**: CPU continuously checks if `flag` is zero before entering critical section. This leads to inefficient usage of CPU cycles, especially if lock is held for a long time by another thread.

- Short interrupt intervals: CPU switches between threads more often. If lock is frequently contested, this can lead to excessive context switching, thus increasing CPU overhead

- Long interrupt intervals: Tasks wait for lock longer -> **increased latency**. CPU spends more time spinlocking too.

Can quantify with CPU utilization. Measure CPU utilization during execution of code; high CPU usage with low system output indicates inefficient usage of CPU (spinlocking)

7) Use the -P flag to generate specific tests of the locking code. For example, run a schedule that grabs the lock in the first thread, but then tries to acquire it in the second. Does the right thing happen? What else should you test?

A: -P 0011. Thread 0 runs first 2 instructions, grabbing the lock in the second instruction. Thread 1 then runs its first 2 instructions, trying to acquire the lock. However, thread 1 fails to do so, hence the right thing happens. Test fairness and performance.

8) Now let's look at the code in peterson.s, which implements Peterson's algorithm (mentioned in a sidebar in the text). Study the code and see if you can make sense of it.

A: Idea is to ensure that 2 threads never enter a critical section at the same time. (Mutual exclusion)


From geeksforgeeks:
Also known as the producer-consumer problem. Two processes - producer and consumer - share a common, fixed size buffer used as a queue. Producers produce an item and put it into a buffer. If buffer is already full then producer wil have to wait for an empty block in the buffer. Consumers consume an item from the buffer. If buffer is already empty then consumers will have to wait for an item in the buffer.

flag[0] = consumer, flag[1] = producer

<u>Key operations</u>
- Flag and Turn variables: Used to control access to critical section
- Busy-Waiting Loops: To check the flag and turn variables to decide if it can enter the critical section

9) Now run the code with different values of -i. What kind of different behavior do you see? Make sure to set the thread IDs appropriately (using -a bx=0,bx=1 for example) as the code assumes it.

A: 
<u>Long interrupt interval</u>
- CPU spends longer intervals executing current code without being interrupted. However, if a thread enters the spin-waiting loops, it may spend more time in the loops, resulting in inefficient CPU usage
- System is less responsive if one thread holds the lock for a long time. The other thread could remain stuck in the spin loop, unable to make progress
- Fairness reduced as a thread holds onto CPU for longer before it is interrupted, resulting in delayed context switch and longer waiting time for other threads, leading to potential starvation.

<u>Low interrupt interval</u>
- Lock contention might be resolved faster as the threads context switch more often, allowing both threads to make progress, reducing the likelihood of long spinning



10) Can you control the scheduling (with the -P flag) to "prove" that the code works? What are the different cases you should show hold? Think about mutual exclusion and deadlock avoidance.

A: -P h

11) Now study the code for the ticket lock in ticket.s. Does it match the code in the chapter? Then run with the following steps: -a bx=1000, bx=1000 (causing each thread to loop through the critical section 1000 times). Watch what happens; do the threads spend much time spin-waiting for the lock?
 
A: Yes. Count is correct (2000, as expected, since total sum of values inside both %bx registers add up to 2000)

When there are only 2 threads, spin-waiting time is minimal.

12) How does the code behave as you add more threads?

A: Each thread will spend more time spin-waiting to wait for its turn, leading to greater ineffiency.

However, ultimately this spin-waiting is fair because the ticket lock ensures that the threads are served in FIFO order.

13) Now examine yield.s, in which a yield instruction enables one thread to yield control of the CPU (realistically, this would be an OS primitive, but for the simplicity, we assume an instruction does the task). Find a scenario where test-and-set.s wastes cycles spinning, but yield.s does not. How many instructions are saved? In what scenarios do these savings arise?

A: Yield instruction = 1 (just yield itself). Hence, # of instructions saved = # of instructions in (n*test-and-set loop) - 1, where n is the number of times the test-and-set loop is ran.

These savings arise when another thread is holding the lock, resulting in a thread spin-waiting (in the case of test-and-set) or the thread yielding the CPU to let another thread run (in the case of yield)

14) Finally, examine test-and-test-and-set.s What does this lock do? What kind of savings does it introduce as compared to test-and-set.s?

A: Set mutex to 1 only if the lock is free. In test-and-set.s, mutex is always set to 1 in when thread tries to acquire lock, regardless of whether the lock was already held or not.

<u>Test-And-Set</u> 
Causes cache line containing the mutex to be marked as modified in the processor's cache, even if lock was already held by another thread. This can lead to a high amount of unnecessary cache coherence traffic ("cache thrashing") on multiprocessor systems because every processor's cache must update their state to reflect their change.

<u>Test-And-Test-And-Set</u>
Reduces the # of expensive atomic operations and unnecessary modifications to the mutex. Minimizes the # of times the cache line containing the mutex is invalidated across processors, significantly reducing cache coherence traffic. Also reduces bus contetion. Betterr scalability with multiprocessors systems. Also increased efficiency as threads that find that the lock is already held can quickly return to spin-wait loop without unnecessary writes.