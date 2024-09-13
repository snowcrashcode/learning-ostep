# Solutions

1) The first problem is just to implement and test a solution to the fork/join problem, as described in the text. Even though this solution is described in the text, the act of typing it in on your own is worthwhile; even Bach would rewrite Vivaldi, allowing one soon-to-be master to learn from an existing one. See `fork-join.c` for details. Add the call sleep(1) to the child to ensure it is working.

Make sure to initialize value of semaphore to be 0.

2) Let's now generalize this a bit by investigating the **rendezvous problem**. The problem is as follows: you have two threads, each of which are about to enter the rendezvous point in the code. Neither should exit this part of the code before the other enters it. Consider using two semaphores for this task, and see `rendezvous.c` for details.

Make sure both s1 and s2 are initialized to 0, so that sem_wait() will immediately block until sem_post() is called from other thread.

<u>Synchronization Mechanism</u>

- sem_wait(&s1) used in child2 will block child2 until s1 is signalled via child1 calling sem_post(&s1)
- Same thing happens for sem_wait(&s2)
- Child1 calls sem_post(&s1) first, allowing child2 to print the "after" message first, followed by child1 

3) Now, go one step further by implementing a general solution to barrier synchronization. Assume there are two points in a sequential piece of code, called P1 and P2. Putting a **barrier** between P1 and P2 guarantees that all threads will execute P1 before any one thread executes P2. Your task: write the code to implement a barrier() function that can be used in this manner. It is safe to assume you know N (the total number of threads in the running program) and that all N threads will try to enter the barrier. Again, you should likely use two semaphores to achieve the solution, and some other integers to count things. See `barrier.c` for details.

A:
Purpose of a barrier is to **ensure that all threads reach the barrier before any of them can proceed**. Once all threads have reached the barrier, they are released to continue executing.
<u>Two Phase Synchronization</u>

1) **Arrival Phase**: Threads wait for all to reach the barrier
2) **Departure Phase**: Threads wait for all to be released from barrier

Execution Flow:
Assume there are N threads.
All N threads will print "child X: before".
Reach Barrier. Context switch to next thread.
Nth thread reaches barrier.
All threads are released to print "child X: after"


4) Now let's solve the **reader-writer problem**, also as described in the text. In this first take, don't worry about starvation. See the code in `reader-writer.c` for details. Add sleep() calls to your code to demonstrate it works as you expect. Can you show the existence of the starvation problem?

Reader-Writer problem: If we can guarantee that no insert(i.e. changing state of data structure, which will result in a critical region) occurs, then we can allow many lookups to occur concurrently, since this will not result in it being a critical section. This allows for more flexible and greater concurrency. To implement this, we use reader-writer lock, where only a single writer can acquire the lock but multiple readers are allowed to acquire the reader lock.

Readers are prioritized over writers. When there is at least one reader in critical section, the writer has to wait until **all reader has finished**. If a reader thread keep entering the critical section before the last reader thread leaves, the writer will be continuously delayed, leading to writer starvation.

5) Let's look at the reader-writer problem again, but this time, worry about starvation. How can you ensure that all readers and writers eventually make progress? See `reader-writer-nostarve.c` for details.

A: Add a readerlock - block readers if there is a waiting writer

6) Use semaphores to build a **no-starve mutex**, in which any thread that tries to acquire the mutex will eventaully obtain it. See the code in `mutex-nostarve.c` for more information.

A:
<u>Ticket Lock: Per-Thread Semaphores</u>

A ticket lock ensures that threads acquire the lock in the order they requested it, preventing starvation.
All threads will be served in order, and no threads will be infinitely delayed.
FIFO order. First thread to obtain a ticket lock will be served first.
Atomic operations `__sync_fetch_and_add`, `__sync_val_compare_and_swap` ensures proper synchronization between threads without race conditions.
Since each thread increments `ticket` atomically and waits for its own ticket number, no thread can skip ahead, and every thread will eventually reach its turn.

Array of semaphores allow each thread to wait for its turn without busy-waiting.


7) Liked these problems? See Downey's free text for more just like them. And don't forget, have fun! But, you always do when you write code, no?