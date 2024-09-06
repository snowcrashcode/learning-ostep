#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        int child_wc = wait(NULL);
        printf("Hello from child %d, wait returns: %d\n", getpid(), child_wc);
    } else {
        // parent process
        int parent_wc = wait(NULL);
        printf("Hello from parent %d, this is what wait returns: %d\n", getpid(), parent_wc);
    }
    return 0;
}