#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

int main() {
    int *x = (int *) malloc(sizeof(int));
    *x = 10;
    printf("%i\n",*x);
    return 0;
}