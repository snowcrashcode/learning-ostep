#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main() {
    int x = 100;
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit process
        fprintf(stderr, "fork failed.\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        x = x + 5;
        printf("Value of x in child process is %d\n", x);

    } else {
        x = x*10;
        int wc = wait(NULL);
        printf("Value of x in parent process is %d\n", x);
        assert(wc>=0);
    }
    return 0;
}

// Call fork()
// Access variable, set its value to 100
// What value is the variable in the child process?
// What happens to the variable when both parents and child change the value of x?