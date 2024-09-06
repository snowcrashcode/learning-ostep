#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

int main() {
    int *data = (int *)malloc(100*sizeof(int));
    data[100] = 0;
    printf("%d\n", data[100]);
    return 0;
}