#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int rc = fork();
    char *args[] = {"ls", "-l", NULL};
    char *env[] = {NULL};
    if (rc < 0 ) {
        // fork failed
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        // execl("/bin/ls", "ls", "-l", (char *) NULL);
        // execle("/bin/ls", "ls", "-l", (char *) NULL, env);
        // execlp("ls", "lp", "-l", (char *) NULL);
        // execv("/bin/ls", args);
        // execvp("ls", args);
        execvpe("ls", args, env);
        printf("This shouldn't print out.\n");
    } else {
        sleep(1);
        printf("Goodbye from parent!\n");
    }
    return 0;
}