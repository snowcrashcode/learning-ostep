#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

/*
Masure costs of system call & context switch
E.g. Call a simple system call (e.g. performing a 0-byte read) & time how long it takes
Divide time by # of iterations will give you the duration of each system call

Precision && accuracy of timer

Timer: gettimeofday()
More precise: rdtsc , available on x86 machines

For context switch
Imbench benchmark: running 2 proccesses on a single CPU, and setting up 2 UNIX pipes between them so that both processes can communicate with each other
First process issues a write to the first pipe, waits for a reado n the second
Upon seeing the first process waiting for something to read from the second pipe, OS puts the first process in the blocked state, and switches to the other process, which reads from the first pipe and then writes to the second
When second pipe tries to read from the first pipe, it gets blocked, and this cycle repeats

For systems with >1 CPU: Ensure that context-switching processes are located on the same processor. E.g. sched_setaffinity(). 
By ensuring both processes are on the same processor, you are making sure that you are measuring the cost of the OS stopping one process and restoring another on the same CPU
*/

int main() {
    int fd[2]; // fd[0] == read end, fd[1] == write end
    char line[1024];
    struct timeval t0, t1;
    long elapsed;

    // create the pipe
    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(EXIT_FAILURE);
    }

    // get initial time
    if (gettimeofday(&t0, NULL) < 0) {
        perror("gettimeofday error");
        exit(EXIT_FAILURE);
    }

    printf("Time at start: %ld.%06ld\n", (long)t0.tv_sec, (long)t0.tv_usec);

    // perform a 0 byte read
    int n = read(fd[0], line, 0);
    if (n<0) {
        perror("read error");
        exit(EXIT_FAILURE);
    }

    // Get time after read
    if (gettimeofday(&t1, NULL) < 0) {
        perror("gettimeofday error");
        exit(EXIT_FAILURE);
    }

    printf("Time at end: %ld.%06ld\n", (long)t1.tv_sec, (long)t1.tv_usec);

    // calculate elapsed time in u seconds
    elapsed = (t1.tv_sec - t0.tv_sec) * 1000000L + (t1.tv_usec - t0.tv_usec);
    printf("Time taken for 0-byte read: %ld microseconds\n", elapsed);
    return 0;
}