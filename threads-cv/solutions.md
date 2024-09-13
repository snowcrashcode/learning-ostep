# Solutions

1) Our first question focuses on `main-two-cvs-while.c` (the working solution). First, study the code. Do you think you have an understanding of what should happen when you run the program?

A:
- Shared Buffer: Array of integers
    - `fill_ptr`: Points to next position to fill (used by producer)
    - `use_ptr`: Points to next position to consume (used by consumer)
    - `num_full`: Tracks how many buffer slots are currently filled

- do_fill(int value): Fills buffer at position pointed to by `fill_ptr` with `value` and increments `fill_ptr` circularly and updates num_full
- do_get(): Retrieves a value from buffer at position pointed to by `use_ptr`; clears buffer position (set it to EMPTY), increments `use_ptr` circularly and decreases num_full
- producer(void *arg): Each producer thread produces a series of unique values based on its `id` and adds them to buffer. If buffer is full, wait on `empty` cv. After adding, signal consumer using `fill` cv.
- consumer(void *arg): Each consumer thread consumes values from buffer until it encounters `END_OF_STREAM` marker. If buffer is empty, wait on `fill` cv. Return # of items it consumed (not counting END_OF_STREAM)


2) Run with one producer and one consumer, and have the producer produce a few values. Start with a buffer (size 1), and then increase it. How does the behaviour of the code change with larger buffers? (or does it?) What would you predict num_full to be with different buffer sizes (e.g., -m 10) and different numbers of produced items (e.g., -l 100), when you change the consumer sleep string from default (no sleep) to -C 0,0,0,0,0,0,1?

A:
With larger buffer size, producer can fill multiple slots in buffer before needing to wait for consumer to empty. Likewise, consumers can consume multiple values in succession before needing to wait for producer to fill the shared buffer. Hence, a larger buffer size will lead to less frequent synchronization and fewer context switches, hence higher throughput overall, increasing efficiency.
With no sleep, num_fill will stay at or near 0, because everytime a producer thread puts a value into a shared buffer (and goes to sleep), a consumer thread is signalled and thus immediately consumes that freshly put value. This allows producer threads to keep filling the shared buffer without much waiting.

After every 7 items consumed, consumer thread goes to sleep, allowing producer thread to fill buffer.
`num_full` slowly increments to buffer size (from 0 to m) as producer threads put values into shared buffer one by one; then, once it reaches the max (assuming # of produced item > m, which 100 is > 7), consumer threads will slowly decrement buffer size one at a time by each getting a value from the shared buffer. Cyclical filling sort of pattern.


3) If posible, run the code on different systems (e.g., a Mac and Linux). Do you see a different behaviour across these systems?

I do not have access to a Mac... Behaviour should depend on the internal implementation of thread schedulers of Linux kernel vs Mach kernel (used by macOS).

4) Let's look at some timings. How long do you think the following execution, with one producer, three consumers, a single-entry shared buffer, and each consumer pausing at point `c3` for a second, will take? `./main-two-cvs-while -p l -c 3 -m l -C 0,0,0,1,0,0,0:0,0,0,1,0,0,0:0,0,0,1,0,0,0 -l 10 -v -t

A:
12.01 seconds.
Between P1 and C1, 10 seconds; C2 and C3 1 additional second each at the end, to check til END_OF_STREAM and then return.
Consumer thread is set to sleep after reacquiring the lock.
Producer produces 10 items, C1 consumes all 10 items every time, C2 and C3 is starved.

5) Now change the size of the shared buffer to 3 (-m 3). Will this make any difference in the total time?

A:  11.01s.
1 second faster.
Number of loops of p/c doesn't change.
Nothing has changed except that at the end, C2 and C3 can check for END_OF_STREAM concurrently, thus the check only takes 1 second instead of 2 seconds, hence the 1 second shaved.

6) Now change the location of the sleep to c6 (this models a consumer taking something off the queue and then doing something with it), again using a single-entry buffer. What time do you predict in this case? ./main-two-cvs-while -p l -c 3 -m l -C 0,0,0,0,0,0,1:0,0,0,0,0,0,1:0,0,0,0,0,0,1 -l 10 -v -t

A: 5 seconds.
Consumer thread calls sleep after releasing the lock, allowing other threads to execute their instructions and get the value from the shared buffer.
 
7) Finally, change the buffer size to 3 again (-m 3). What time do you predict now?

A: No change in timing, still 5 seconds.

8) Now let's look at main-one-cv-while.c. Can you configure a sleep string, assuming a single producer, one consumer, and a buffer of size 1, to cause a problem with this code?

A: No. Unless a thread spuriously wakes up i.e., it is waiting on a condition variable and is awakened even though no thread has signalled the condition variable and the condition it was waiting for is still false. E.g., `0,0,0,0,0,0,1`, if consumer sleeps after singalling producer, and producer does not wake up immediately (due to missing/delayed signal), then both threads may end up in a waiting state without a clear signal to each other.

9) Now change the number of consumers to two. Can you construct sleep strings for the producer and the consumers so as to cause a problem in the code?

A:
`./main-one-cv-while -c 2 -v -P 0,0,0,0,0,0,1`
Producer goes to sleep after putting value in shared buffer and releasing lock.
Producer remains blocked after consumer runs. Deadlock (all 3 threads are waiting indefinitely)

10) Now examine main-two-cvs-if.c. Can you cause a problem to happen in this code? Again consider the case where there is only one consumer, and then the case where there is more than one.

A: Just always use a `while` loop. Single consumer thread is fine. However, if there are more than 1 consumer thread, a while loop will be able to handle the case where spurious wakeups occur. It is possible that two or more threads get woken up by a single signal. Hence, we should use a while loop to recheck condition that a thread is waiting on, in case the state has last changed since the thread was awoken (due to other consumer threads running).

11) Finally, examine `main-two-cvs-while-extra-unlock.c`. What problem arises when you release the lock before doing a put or a get? Can you reliably cause such a problem to happen, given the sleep strings? What bad thing can happen?

A:
Data race can occur as do_fill and do_get should be critical sections, but they are not locked. Multiple threads can access and modify shared variables simultaneously, thus causing inconsistent/incorrect states.

Possible to modify sleep string such that when you have multiple consumer threads, a consumer thread only consumes one/zero value (starvation)