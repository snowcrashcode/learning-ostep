#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

#include "main-header.h"
#include "vector-header.h"

int retry = 0;

void vector_add(vector_t *v_dst, vector_t *v_src) {
  top:
    if (pthread_mutex_trylock(&v_dst->lock) != 0) { // If destination lock is not availabe, non-zero value is returned, go back to top
	goto top; // Ensures thread can proceed only if it has successfully acquired destionation lock
    }
    if (pthread_mutex_trylock(&v_src->lock) != 0) { // If source lock is not available, non-zero value is returned, go back to top
	retry++;
	Pthread_mutex_unlock(&v_dst->lock); // unlock destionation lock
	goto top;
    }
    int i;
    for (i = 0; i < VECTOR_SIZE; i++) {
	v_dst->values[i] = v_dst->values[i] + v_src->values[i];
    }
    Pthread_mutex_unlock(&v_dst->lock);
    Pthread_mutex_unlock(&v_src->lock);
}

void fini() {
    printf("Retries: %d\n", retry);
}

#include "main-common.c"

