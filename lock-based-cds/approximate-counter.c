#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>

#define NUMCPUS 12 // #define is a pre-processor directive, hence at compile time, NUMCPUS is a constant

typedef struct __counter_t {
    int global; // global count
    pthread_mutex_t glock; // global lock
    int local[NUMCPUS];
    pthread_mutex_t llock[NUMCPUS];
    int threshold; // update freq;
} counter_t;

// Used to pass argument to thread function
typedef struct __thread_arg {
    int cpu_idx; // index of CPU core that the thread will run on
    pthread_t thread; // represents the thrread
    counter_t *counter; // a pointer to the shared counter structure
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
    
    // Increment the counter many times using the approximate counting method
    for (int i = 0; i < 1000000; i++) {  // adjust the iteration count as needed
        update(arg->counter, arg->cpu_idx, 1); // call update function to increment
    }
    return NULL;
}

// init: record threshold, init locks, init values of all local counts and global count
void init(counter_t *c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init (&c->glock, NULL);
    for (int i = 0; i < NUMCPUS; i++) {
        c->local[i] = 0;
        pthread_mutex_init(&c -> llock[i], NULL);
    }
}

// update: usually, just grab local lock and update
// local amount; once it has risen `threshold`, grab global lock and transfer local values to it
void update(counter_t *c, int threadID, int amt) {
    int cpu = threadID % NUMCPUS;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold) {
        // transfer to global (assume amt > 0)
        pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}

// get: just return global amount (approximate)
int get(counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
}

int main() {
    counter_t *c = malloc(sizeof(counter_t));
    if (c == NULL) {
        printf("Failed to allocate memory for counter.\n");
        exit(EXIT_FAILURE);
    }

    int thresholdVal = 1024; // can vary this value, as per question
    init (c, thresholdVal);

    struct timeval time_before, time_after; 

    for (int i = 1; i <= NUMCPUS; ++i) {
        thread_arg *args = malloc(i * sizeof(thread_arg));
        if (args == NULL) {
            printf("Failed to allocate memory space for thread arguments.\n");
            exit(1);
        }
        
        // Reset counter
        c->global = 0;

        // Record the time before starting threads
        gettimeofday(&time_before, NULL);

        // Create threads
        for (int j = 0; j < i; j++) {
            args[j].cpu_idx = j % NUMCPUS;  // assign a CPU core to each thread
            args[j].counter = c; // initialize counter pointer
            pthread_create(&args[j].thread, NULL, thread_function, &args[j]); // create thread
        }

        // After all creates are created, it waits for eadch thread to finish by calling join() 
        for (int j = 0; j < i; j++) {
            pthread_join(args[j].thread, NULL);
        }
        int global_val = get(c);

        // Record the time after threads finished
        gettimeofday(&time_after, NULL);

        double elapsed_time = (time_after.tv_sec - time_before.tv_sec) + 
                              (time_after.tv_usec - time_before.tv_usec) / 1e6; // calculate elapsed time in seconds

        printf("Running %d threads..\n", i); // i === loop iteration # === number of threads used
        printf("Counter value: %d\n", global_val); // final counter value (should be correct; no data race should occur)
        printf("Time elapsed: %.6f s\n\n", elapsed_time);
        printf("Threshold used: %d\n", thresholdVal);

        free(args);  // Free memory for thread arguments

    }
    free(c); // free counter memory

    return 0;
}