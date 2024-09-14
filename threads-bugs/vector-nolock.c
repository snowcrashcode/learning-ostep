#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

#include "main-header.h"
#include "vector-header.h"

// taken from https://en.wikipedia.org/wiki/Fetch-and-add
int fetch_and_add(int * variable, int value) {
    asm volatile("lock; xaddl %%eax, %2;" // lock ensures atomicity; %2 refers to second input argument
		 :"=a" (value) // Result of xaddl is put into %eax register, which is mapped onto `value` variable 
		 :"a" (value), "m" (*variable)  // value is initially loaded into %eax register;
		 :"memory"); // Memory Cobbler. Tells compiler that assembly code modifies memory, so it shouldn't assume anything about the contents of memory and should reload any values that might have been modified.
    return value; // Returns original value of *variable
}

void vector_add(vector_t *v_dst, vector_t *v_src) {
    int i;
    for (i = 0; i < VECTOR_SIZE; i++) {
	fetch_and_add(&v_dst->values[i], v_src->values[i]); // Exchanges value from source to destination 
    }
}

void fini() {}


#include "main-common.c"

