#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
    int rc = fork();
    int status, pid;
    int fd[2]; // represents the pipe. fd[0] is the read end; fd[1] is the write end.
    char line[1024];

    if(pipe(fd) < 0) printf("pipe error\n");

    if (rc < 0) {
        // fork failed
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // child processes
        int rc2 = fork(); // fork again
        if (rc2 == 0) {
            // child. This is executed second
            close(fd[1]); // closes write end of pipe
            printf("Received message from another child %d\n", getpid()); // indicates it received a message from another child
            int n = read(fd[0], line, 1024); // reads from read end of pipe
            write(STDOUT_FILENO, line, n); // writes the read message to STDOUT
            exit(0);
        } else {
            // Parent. This is executed first. Actually the first child of the original process
            close(fd[0]); // closes the read end of pipe
            write(fd[1], "Hello from one child\n", 21); // writes a message to the write end of the pipe
            exit(0);
        }
    } 
}

// fd[0] for reading, fd[1] for writing
// pipe(fd) initializes both 2 fds, such that data written to fd[1] can be read from fd[0]
// Allows for unidirectional communication between process
