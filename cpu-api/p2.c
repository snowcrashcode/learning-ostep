#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("Hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc==0) {
        // child (new process)
        // not an exact copy
        printf("Hello, I am child (pid:%d)\n", (int) getpid());
        sleep(1);
    } else {
        // parent goes down this path (original process)
        // parent process calls wait() to delay its execution until the child finishes executing.
        // When child is done, wait() returns to the parent
        int wc = wait(NULL);
        printf("Hello, I am parent of %d (wc:%d) (pid:%d)\n", rc, wc, (int) getpid());
    }
    return 0;

}

// First, (parent) process runs main()
// fork() is called, creating a child process
// Here, it splits into 2 possible cases
// (1) Child process runs first i.e. rc == 0
// Child prints, executes finishes
// parent exeuctes, finishes
// (2) Parent process runs first, i.e. rc == pid+1
// wait() is called
// child process runs, executes finish
// system call returns to return wc in parent process
// parent process finishes
// hence, adding the wait() makes the process deterministic