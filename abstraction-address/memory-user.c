#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>



int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: memory-user <memory> <optional:time>\n");
        exit(EXIT_FAILURE);
    }

    printf("pid:%d\n", getpid()); // check pid of process run


    int memory = atoi(argv[1]) * 1024 * 1024; // # of megabytes of memory it will use. Mega = 1024*1024, assuming it is in binary, not decimal.
    int *arr = calloc(memory/ sizeof(int), sizeof(int)); // initialize array
    int runTime = atoi(argv[2]); // how long will the program run through the array
    int length = (int) (memory/sizeof(int)); // number of elements in array. Take total memory of array, divide by size of each element in array.
    if (argc == 3) {
        clock_t begin = clock(); // start timer
        double time_spent;
        while (1) {
            time_spent = (double) clock() - begin;
            if (time_spent >= runTime) break;
            for (int i = 0; i < length; ++i) {
                arr[i] += 1;
            }
            printf("%i\n", arr[0]);
        }
        free(arr); // remember to free the memory allocated when creating the array
        return 0;
    }
    // process will run indefinitely since user did not specify duration
    while (1) {
        for (int i = 0; i < length; ++i) {
            arr[i] += 1;
        }
    }
    return 0;
    
    

}