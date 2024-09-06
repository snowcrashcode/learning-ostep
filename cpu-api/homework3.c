#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

    char childContent[] = "hello\n";
    char parentContent[] = "goodbye\n";
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        printf("%s", childContent);
    } else {
        sleep(10);
        printf("%s", parentContent);
    }
    return 0;
}
// try to make child call first without using wait()