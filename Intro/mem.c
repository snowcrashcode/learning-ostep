// A program that access memory

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: mem <value>\n");
        exit(1);
    }
    int *p;
    p = malloc(sizeof(int)); // allocate some memory
    assert(p != NULL);
    printf("(%d) addr pointed to by p: %p\n", (int) getpid(), p); // print out the address
    *p = atoi(argv[1]); // assign value to addr stored in p
    *p = 0; // Puts the number 0 into the first slot of the newly allocated memory
    while (1) {
        Spin(1);
        *p = *p + 1;
        printf("(%d) value of p: %d\n", getpid(), *p);
    }
    return 0;
}