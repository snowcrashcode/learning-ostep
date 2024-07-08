#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Hello world (pid:%d)\n", (int) getpid());
    // child process starts here vvvvv; not main()!
    int rc = fork();
    // For parent process, rc > 0 (usually pid + 1)
    // for child process, rc == 0
    printf("%d\n", rc);
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    } else {
        // parent goes down this path (original process)
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}
// parent process
// parent process terminates
// child process
// child process terminates