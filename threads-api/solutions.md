# 1
First build `main-race.c`. Examine the code so you can see the (hopefully obvious) data race in the code. Now run helgrind (by typing valgrind --tool=helgrind main-race) to see how it reports the race. Does it print the right lines of code? What other information does it give to you?

A:
Data race is present. `balance` is the shared variable. In line 8, a worker thread attempts to update balance, and in line 15, main thread attempts to update the balance too.

Helgrind prints the right lines of code correctly (8 and 15), the conflict address and conflict size, the fact that no locks are currently held, and that the shared variable is "balance".

# 2
What happens when you remove one of the offending lines of code? Now add a lock around one of the updates to the shared variable, and then around both. What does helgrind report in each of these cases?

A: No error.
After removal, if there is only one lock around one of the updates, helgrind will report that there is a possible data race. If you add a lock around both of the updates to the shared variable, then helgrind reports that there is no errors.


When 2 threads are updating a shared variable and only 1 thread is locked while the other isn't:   

Assume thread A is locked and B is unlocked. While A has the lock, no other thread can try to acquire the lock and access the shared variable.
However, since B is not locked, B can access the shared variable anytime without waiting to acquire the lock.
Hence a data race occurs as B can access and modify the shared variable, regardless of the fact that A is doing it with a lock.

In short, all threads accessing a shared variable must be locked, else a data race is present.

# 3
Now let's look at `main-deadlock.c`. Examine the code. This code has a problem known as **deadlock** (which we discuss in much more depth in a forthcoming chapter). Can you see what problem it might have?

A: 2 threads lock each other

# 4
Now run `helgrind` on this code. What does helgrind report?

A: lock order violated

Observed (incorrect) order is: acquisition of lock at...
followed by a later acquisition of lock at...
Required order was established by acquisiton of lock at .. followed by a later acquisition of lock at ..


# 5
Now run `helgrind` on `main-deadlock-global.c`. Examine the code; does it have the same problem that `main-deadlock.c` has? Should `helgrind` be reporting the same error? What does this tell you about tools like `helgrind`?

A: It doesn't have the same problem as `main-deadlock.c` due to the presence of a global lock. However, `helgrind` is still reporting the same error. This tells me that helgrind is potentially unable to detect presence of a global lock is preventing a deadlock from occuring.

# 6
Let's next look at `main-signal.c`. This code uses a variable (done) to signal that the child is done and that the parent can now continue. Why is this code inefficient? (What does the parent end up spending its time doing, particularly if the child takes a long time to complete?)

A: Parent thread remains in a wait status as its constantly checking if the `done` variable is no longer 0 (line 16). Being in wait status itself still requires CPU resources, and this is inefficient. 

# 7
Now run `helgrind` on this program. What does it report? Is the code correct?

A: Possible data race

# 8
Now look at a slightly modified version of the code, which is found in `main-signal-cv.c`. This version uses a condition variable to do the signalling (and associated lock). Why is this code preferred to the previous version? Is it correctness, performance, or both?

A: Both. Condition variables lead to improved performance as CPU won't waste cycles on spinning for a long time to check if a variable has been updated. Condition variables are also less error prone compared to using flags to synchronize threads

# 9
Once again run `helgrind` on `main-signal-cv`. Does it report any errors?

A: No errors