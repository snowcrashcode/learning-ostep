#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>




/*
For context switch
Imbench benchmark: running 2 proccesses on a single CPU, and setting up 2 UNIX pipes between them so that both processes can communicate with each other
First process issues a write to the first pipe, waits for a reado n the second
Upon seeing the first process waiting for something to read from the second pipe, OS puts the first process in the blocked state, and switches to the other process, which reads from the first pipe and then writes to the second
When second pipe tries to read from the first pipe, it gets blocked, and this cycle repeats

For systems with >1 CPU: Ensure that context-switching processes are located on the same processor. E.g. sched_setaffinity(). 
By ensuring both processes are on the same processor, you are making sure that you are measuring the cost of the OS stopping one process and restoring another on the same CPU
*/

// define CPU affinity functions
void set_cpu_affinity(int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // create UNIX pipes
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("pipe error\n");
        exit(EXIT_FAILURE);
    }

    // fork processes
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork erro");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        set_cpu_affinity(0); // pin to CPU 0
        close(pipe1[0]); // close read end of pipe1
        close (pipe2[1]); // close write end of pipe2

        // write to pipe1 and read from pipe2 in a loop for multiple iterations
        for (int i = 0; i < 1000; ++i) {
            if (write(pipe1[1], "msg", 3) != 3) {
                perror("Write error.\n");
                exit(EXIT_FAILURE);
            }
            char buffer[3];
            if (read(pipe2[0], buffer, 3) != 3) {
                perror("Read error\n");
                exit(EXIT_FAILURE);
            }
        }

        close(pipe1[1]); // close write end of pipe1
        close(pipe2[0]); // close read end of pipe2
        exit(0);
    }
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("Fork error.\n");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // child process 2
        set_cpu_affinity(0); // pin to cpu 0
        close(pipe1[1]); // close write end of pipe1
        close (pipe2[0]); // close read end of pipe2

        // write to pipe2 and read from pipe1 in a loop for multiple iterations
        for (int i = 0; i < 1000; ++i) {
            char buffer[3];
            if (read(pipe1[0], buffer, 3) != 3) {
                perror("Read error.\n");
                exit(EXIT_FAILURE);
            }
            if (write(pipe2[1], buffer, 3) != 3) {
                perror("Write error.\n");
                exit(EXIT_FAILURE);
            }
        }
        close(pipe1[0]);
        close(pipe2[1]);
    }
    // parent process
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // wait for both processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Context switch performance measurement completed.\n");
    return 0;
    

}