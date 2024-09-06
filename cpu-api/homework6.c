#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    int rc = fork();
    int status, pid;
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        printf("Hello from child process, %d\n", getpid());
    } else {
        // parent process
        pid_t parent_wcpid = waitpid(rc, &status, WUNTRACED);
        if(WIFEXITED(status)) {
            printf("Normal termination of child %d, exit status = %d\n", rc, WEXITSTATUS(status));
        }
        printf("Hello from parent, return value is %d\n", parent_wcpid);
    }
    return 0;
}

// waitpid(pid, status_ptr, options)
// pid: Specifies the child processes the claler wants to wait for
// status_ptr: Points to a location where waitpid() can store a status value

// observations: waitpid() returns pid of child process