#include <stdio.h>

#include "common_threads.h"

int balance = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* arg) {
    Mutex_lock(&lock);
    balance++; // unprotected access 
    Mutex_unlock(&lock);
    return NULL;
}


int main(int argc, char *argv[]) {
    pthread_t p;
    Pthread_create(&p, NULL, worker, NULL);
    Mutex_lock(&lock);
    balance++;
    Mutex_unlock(&lock);
    Pthread_join(p, NULL);
    return 0;
}
