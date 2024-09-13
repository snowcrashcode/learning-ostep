#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common_threads.h"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
    // add semaphores and other information here
    sem_t s1;
    sem_t s2;
    sem_t mutex;
    int threadsQueued;
    int threadsTotal;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    // initialization code goes here
    sem_init(&b->mutex, 0, 1); // mutex semaphore is usually initialized to 1, so that u can lock (wait) it and value will still be 0, hence allowing thread to immediately run after acquiring lock
    sem_init(&b->s1, 0, 0);
    sem_init(&b->s2, 0, 0);
    b->threadsTotal = num_threads;
    b->threadsQueued = 0;
}

void barrier(barrier_t *b) {
    // barrier code goes here
    
    // First Phase: Arrival of threads
    // critical region: if queue of threads at barrer is full, signal to semaphore1 so that it can execute child one by one
    sem_wait(&b->mutex); // acquire lock. Updating of threadsQueud must be atomic.
    b->threadsQueued++;
    if (b->threadsTotal == b->threadsQueued) { // all threads of the program are now waiting at the barrier
        for (int i = 0; i < b->threadsTotal; ++i) {
            sem_post(&b->s1); // wake up barrier's first semaphore
        }
    }
    sem_post(&b->mutex);
    sem_wait(&b->s1); // each thread is waiting on s1, which blocks all threads until the last thread as arrived and wakes them up (line 44's for loops) 

    // Second Phase: Departure of threads
    // critical region :  
    sem_wait(&b->mutex);
    b->threadsQueued--;
    if (b->threadsQueued == 0) { // when last thread is leaving barrier
        for (int i = 0; i < b->threadsTotal; ++i) {
            sem_post(&b->s2); // All thread waits on s2 to ensure that they don't leave the barrier prematurely, thus synchronizing their departure
        }
    }
    sem_post(&b->mutex);
    sem_wait(&b->s2);
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}


// run with a single argument indicating the number of 
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);
    
    int i;
    for (i = 0; i < num_threads; i++) {
	t[i].thread_id = i;
	Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++) 
	Pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}

