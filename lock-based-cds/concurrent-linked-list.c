#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

// Basic node structure
typedef struct __node_t {
    int key;
    struct __node_t *next;
} node_t;

// Basic list structure (one used per list)
typedef struct __list_t {
    node_t *head;
    pthread_mutex_t lock;
} list_t;

void List_Init(list_t *L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

int List_Insert(list_t *L, int key) {
    // synchronisation not needed
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return -1;
    }
    new->key = key;
    // Just lock critical section
    pthread_mutex_lock(&L->lock);
    new->next = L->head;
    L->head = new;
    pthread_mutex_unlock(&L->lock);
    return 0; // success
}

int List_Lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr) {
        if (curr->key == key) {
            rv = 0;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv; // now both success and failure
}

int main() {
    list_t *lst = malloc(sizeof(list_t));
    if (lst == NULL) {
        printf("Failed to allocate memory for list.\n");
        exit(EXIT_FAILURE);
    }
    List_Init(lst);

    // Lets measure the time taken to insert 100 nodes
    struct timeval time_before, time_after; 

    // Initialize 1e6 list nodes with keys 0-1e6-1 
    for (int i = 0; i < 1e6; i++) {
        List_Insert(lst, i);
    }

    // Record the time before inserting 100 nodes 
    gettimeofday(&time_before, NULL);

    // Time to lookup list to find if node key 500000 exists (it shoul.d)
    int returnVal = List_Lookup(lst, 500000); // Lookup key 500000
    if (returnVal == 0) {
        printf("Key 500000 found in the list.\n");
    } else {
        printf("Key 500000 not found in the list.\n");
    }
    
    // Record the time after initialization is finished 
    gettimeofday(&time_after, NULL);

    double elapsed_time = (time_after.tv_sec - time_before.tv_sec) + 
                            (time_after.tv_usec - time_before.tv_usec) / 1e6; // calculate elapsed time in seconds
    printf("Time elapsed: %.6f s\n\n", elapsed_time);
    free(lst); // free counter memory
    
    return 0;
}