#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

#include "main-header.h"
#include "vector-header.h"

// Vector addition
// destination vector == The result vector
// source vector == The vector that we want to add
// Yes, this function itself is useless. 
void vector_add(vector_t *v_dst, vector_t *v_src) {
    Pthread_mutex_lock(&v_dst->lock); // Lock destination lock
    Pthread_mutex_lock(&v_src->lock); // Lock source lock
    int i;
    for (i = 0; i < VECTOR_SIZE; i++) {
        // Iterate through entire vector. In each iteration, sum up destination vector's row value with source vector's row value and put it into destination vector
        // Think of the for loop as adding row by row
	v_dst->values[i] = v_dst->values[i] + v_src->values[i];
    }
    Pthread_mutex_unlock(&v_dst->lock); // unlock destination lock
    Pthread_mutex_unlock(&v_src->lock); // unlock source lock
}

void fini() {}

#include "main-common.c"
