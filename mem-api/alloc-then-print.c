#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

int main() {
    int *x = (int *) malloc(100*sizeof(int));
    free(x);
    printf("%d\n", x[19]);
    return 0;
}