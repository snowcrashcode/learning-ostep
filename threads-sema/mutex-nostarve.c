#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

#define MAX_THREADS 100 // Max number of threads supported

typedef struct __ns_mutex_t {
    volatile int ticket; // Next available ticket number
    volatile int serving; // Ticket number currently being served
    sem_t mutex;
    sem_t sems[MAX_THREADS]; // Array of semaphores for each thread
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    m->ticket = 0;
    m->serving = 0;
    sem_init(&m->mutex, 0, 1);
    for (int i = 0; i < MAX_THREADS; ++i) {
        sem_init(&m->sems[i], 0, 0); // Initialize all pre-thread semaphores
    }
    // Start mutex by releasing semaphore for ticket 0
    sem_post(&m->sems[0]);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    sem_wait(&m->mutex);
    int my_ticket = m->ticket & MAX_THREADS; // Prevents array index overflow by ensuring the ticket numbers wrap around (the max number of threads, hence size of semaphore array) correctly
    m->ticket++;
    sem_post(&m->mutex);

    // Wait on semaphore corresponding to this thread's ticket number
    sem_wait(&m->sems[my_ticket]);
}

void ns_mutex_release(ns_mutex_t *m) {
    sem_wait(&m->mutex);
    m->serving++;
    int next = m->serving % MAX_THREADS;
    sem_post(&m->mutex);
    // Signal semaphore corresponding to the ticket number that is about to be served
    sem_post(&m->sems[m->serving]);
}


void *worker(void *arg) {
    ns_mutex_t *mutex = (ns_mutex_t *) arg;
    for (int i = 0; i < 3; ++i) { // Each thread tries to acquire mutex
        ns_mutex_acquire(mutex);
        printf("Thread %ld acquired the lock\n", pthread_self());
        sleep(1); // Simulate work
        printf("Thread %ld releasing lock\n", pthread_self());
        ns_mutex_release(mutex);
        sleep(1); // Simulate time before trying again
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    ns_mutex_t mutex;
    ns_mutex_init (&mutex);

    int num_threads = 5;
    pthread_t threads[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; ++i) {
        Pthread_create(&threads[i], NULL, worker, &mutex);
    }

    // Join threads
    for (int i = 0; i < num_threads; ++i) {
        Pthread_join(threads[i], NULL);
    }
    printf("parent: end\n");
    return 0;
}

