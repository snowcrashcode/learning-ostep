#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


int main() {

    int fd = open("output.txt", O_WRONLY|O_CREAT|O_APPEND);
    if (fd == -1) {
        printf("Error Number %d\n", errno);
        perror("Program");
        exit(1);
    }
    int rc = fork();
    char buf1[] = "content1\n";
    char buf2[] = "content2\n";
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        write(fd, buf1, sizeof(buf1) - 1);
        exit(0);

    } else {
        // parent process
        write(fd, buf2, sizeof(buf2) - 1);
    }
}
// open a file
// create a new process
// Can both the child and parent access the file descriptor returned by open()?
// What happens when they are writing to the file concurrently, i.e. at the same time?