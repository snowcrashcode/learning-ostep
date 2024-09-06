### 1) Segmentation fault. Program is attempting to access memory that it doesn't have permission to access. Dereferencing a NULL pointer is undefined behaviour.

### 2) gcc -o null -g null.c : `-o null` to name output executable null; `-g` to include debugging information in null executable; `null.c` is the source file.   
Gdb shows:    
Program received signal SIGSEGV, Segmentation fault.
0x000055555555518f in main () at null.c:11
11      printf("%d\n", *x);

### 3) 
==102699== Memcheck, a memory error detector
==102699== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==102699== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==102699== Command: ./null
==102699== 
==102699== Invalid read of size 4
==102699==    at 0x10918F: main (null.c:11)
==102699==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==102699== 
==102699== 
==102699== Process terminating with default action of signal 11 (SIGSEGV)
==102699==  Access not within mapped region at address 0x0
==102699==    at 0x10918F: main (null.c:11)
==102699==  If you believe this happened as a result of a stack
==102699==  overflow in your program's main thread (unlikely but
==102699==  possible), you can try to increase the size of the
==102699==  main thread stack using the --main-stacksize= flag.
==102699==  The main thread stack size used in this run was 8388608.
==102699== 
==102699== HEAP SUMMARY:
==102699==     in use at exit: 4 bytes in 1 blocks
==102699==   total heap usage: 1 allocs, 0 frees, 4 bytes allocated
==102699== 
==102699== LEAK SUMMARY:
==102699==    definitely lost: 4 bytes in 1 blocks
==102699==    indirectly lost: 0 bytes in 0 blocks
==102699==      possibly lost: 0 bytes in 0 blocks
==102699==    still reachable: 0 bytes in 0 blocks
==102699==         suppressed: 0 bytes in 0 blocks
==102699== Rerun with --leak-check=full to see details of leaked memory
==102699== 
==102699== For lists of detected and suppressed errors, rerun with: -s
==102699== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
Segmentation fault

My takeaway: 4 bytes of memory was leaked. Invalid read of Addres 0x0 (null) is referring to the undefined dereferencing of an int pointer to NULL. Furthermore, 1 alloc and 0 free is not correct; I need to free() the int pointer that I had malloc'ed.

### 4) Program runs as expected. No memory leak is visible to the user in the output. Classic case of "Just because a program runs or compiles doesn't mean it's correct/bug-free!".  
GDB is unable to debug any bugs. Cannot detect memory leak, since after process exits, OS would reclaim all address spaces, even if there was a memory leak on the heap.
valgrind is able to find the memory leak. Output tells me (debugger) that there are definitely 4 bytes lost under the leak summary.

### 5) Program runs "correctly" and outputs zero. Valgrind is able to find the memory leak, giving "Invalid write of size 4" and that I definitely lost 400 bytes under "LEAK SUMMARY".  

Program seemingly runs correctly as malloc() typically allocates memory contiguously, and OS provides more memory than strictly requested. However, these may lead to undefined behaviour, memory corruption, or security risks.

### 6) Still able to print out a value. Valgrind gives "invalid read of size 4", "all heap blocks were freed -- no leaks are possible" and that 0 bytes were leaked.


### 7) This givese an error. free(): invalid pointer Aborted. No extra tools are needed. This is a run time error.

