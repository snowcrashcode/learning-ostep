#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

int main() {
    int *x = (int *) malloc(sizeof(int));
    x = NULL;
    printf("%d\n", *x);
    return 0;
}

