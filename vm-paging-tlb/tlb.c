#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

// Roughly measure the cost of accessing each page.
// Inputs: Number of pages to touch and number of trials


int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s pages trials\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    long PAGESIZE = (size_t)sysconf(_SC_PAGESIZE); // 4096
    int NUMPAGES = atoi(argv[1]);
    int trials = atoi(argv[2]);

    if (NUMPAGES <= 0 || trials <= 0) {
        fprintf(stderr, "Number of pages and trials must be positive. Invalid input.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize array tlb, which is an array (linear data structure) used to represent the translation-lookaside buffer
    int *tlb = calloc(NUMPAGES, PAGESIZE); // number of pages is the number of elements in the array; pagesize is the size of each page
    long jump = PAGESIZE / sizeof(int); // 1024. Used to jump across each page to access the next array element

    struct timespec {
        time_t tv_sec; // seconds
        long tv_nsec; // nanoseconds
    } start, end; 


    __clockid_t clock = CLOCK_PROCESS_CPUTIME_ID;
    if ((clock_gettime(clock, &start)) == -1) {
        perror("start clock_gettime returned -1.\n");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < trials; ++j) {
        for (int i = 0; i < NUMPAGES * jump; i+= jump) {
            tlb[i] += 1;
        }
    }

    if (clock_gettime(clock, &end) == -1) {
        perror("end clock_gettime() returned -1.\n");
        exit(EXIT_FAILURE);
    }
    // print results.
    printf("%f\n", ((end.tv_sec - start.tv_sec) * 1E9 + end.tv_nsec - start.tv_nsec) / (trials*NUMPAGES));

    if (clock_getres(clock, &end) == -1) {
        perror("clock_getres");
        exit(EXIT_FAILURE);
    }
    // printf("Resolution: %10ld.%09ld\n", (long) end.tv_sec, end.tv_nsec);

    free(tlb);
    return 0;
}

/*
int clock_getres(clockid_t clockid, struct timespec *res); -> finds precision of specified clock clockid, and if res is non-NULL, stores it in the struct timespec pointed to by res
int clock_gettime(clockid_t clockid, struct timespec *tp);

CLOCK_PROCESS_CPUTIME_ID : Measures CPU time consumed by this process (i.e., CPU time consumed by all threads in the process)
*/