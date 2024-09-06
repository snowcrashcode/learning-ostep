#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Represents the shared counter
typedef struct __counter {
    int count;
    pthread_mutex_t lock; // Protect concurrent access to `count` by multiple threads
} counter;

// Used to pass argument to thread function
typedef struct __thread_arg {
    int cpu_idx; // index of CPU core that the thread will run on
    pthread_t thread; // represents the thrread
    counter *counter; // a pointer to the shared counter structure
} thread_arg;


// Thread function to increment the counter; Each thread will execute this function
void *thread_function(void *args) {
    thread_arg *arg = (thread_arg *) args; // initialize thread arguments to thread_arg *arg  
    
    // Set CPU affinity for the thread
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(arg->cpu_idx, &cpu_set);
    // set CPU affinity -> Forces thread to run on a specific CPU core based on cpu_idx
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) != 0) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
    
    // Increment the counter many times
    for (int i = 0; i < 1000000; i++) {  // adjust the iteration count as needed
        pthread_mutex_lock(&arg->counter->lock); // Each increment is protected by a mutex to prevent data race
        arg->counter->count++;
        pthread_mutex_unlock(&arg->counter->lock);
    }

    return NULL;
}

// Initialize the counter
void init(counter *c) {
    c->count = 0; 
    pthread_mutex_init(&c->lock, NULL); // initalize mutex lock associated with counter
}

int main(int argc, char *argv[]) {
    counter *c = malloc(sizeof(counter));
    if (c == NULL) {
        printf("Failed to allocate memory space for counter.\n");
        exit(1);
    }
    init(c);

    struct timeval time_before, time_after;
    
    // Get the number of logical cores (divide by 2 if hyper-threading is enabled)
    // Retrieve number of logical CPU cores using sysconf, which is the maximum number of threads the program will test
    int num_of_cores = sysconf(_SC_NPROCESSORS_ONLN);

    // Test with different numbers of threads, strating from 1 going up to the # of logical cpu cores
    for (int i = 1; i <= num_of_cores; ++i) {
        thread_arg *args = malloc(i * sizeof(thread_arg));
        if (args == NULL) {
            printf("Failed to allocate memory space for thread arguments.\n");
            exit(1);
        }
        
        // Reset counter
        c->count = 0;

        // Record the time before starting threads
        gettimeofday(&time_before, NULL);

        // Create threads
        for (int j = 0; j < i; j++) {
            args[j].cpu_idx = j % num_of_cores;  // assign a CPU core to each thread
            args[j].counter = c; // initialize counter pointer
            pthread_create(&args[j].thread, NULL, thread_function, &args[j]); // create thread
        }

        // After all creates are created, it waits for eadch thread to finish by calling join() 
        for (int j = 0; j < i; j++) {
            pthread_join(args[j].thread, NULL);
        }

        // Record the time after threads finished
        gettimeofday(&time_after, NULL);

        double elapsed_time = (time_after.tv_sec - time_before.tv_sec) + 
                              (time_after.tv_usec - time_before.tv_usec) / 1e6; // calculate elapsed time in seconds

        printf("Running %d threads..\n", i); // i === loop iteration # === number of threads used
        printf("Counter value: %d\n", c->count); // final counter value (should be correct; no data race should occur)
        printf("Time elapsed: %.6f s\n\n", elapsed_time);

        free(args);  // Free memory for thread arguments
    }

    free(c);  // Free the counter memory
    return 0;
}
