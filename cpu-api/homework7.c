#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // Child process
        close(STDOUT_FILENO);
        printf("Im guessing that this wont be printed out.\n");
        exit(0);


    } else {
        // parent process
        printf("Hello from parent process.\n");
    }
    return 0;
}

// 1) File descriptor for STDOUT_FILENO (which is typically 1) is closed, hence fd of 1 is no longer associated with any terminal or file
// 2) printf() tries to write to the fd of 1. However, since fd(1) has been closed, there is no valid fd to write to
// 3) Output from printf() is effectively discarded. printf() uses the write() system call internally, which fails because there is no valid fd
// 4) Printf() itself does not directly indicate an error if write fails. It returns the # of chars it attempted to write, not the # of chars actually written
// However, underlying write() returns -1 and set errno to EBADF