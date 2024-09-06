#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// Defining vector structure
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Vector;

// Initialize vector
void vector_init(Vector *vector) {
    vector->size = 0;
    vector->capacity = 1; // start with small capcity
    vector->data=  malloc(vector->capacity*sizeof(int));
    if (vector->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

// add element to vector
void vector_add(Vector *vector, int value) {
    // if vector is full, double its capacity
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->data = (int *)realloc(vector->data, vector->capacity * sizeof(int));
        if (vector->data == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    vector->data[vector->size] = value;
    vector->size++;
}

int vector_get (Vector *vector, size_t index) {
    if (index >= vector->size) {
        fprintf(stderr, "Index out of bounds.\n");
        exit(EXIT_FAILURE);
    }
    return vector->data[index];
}

void vector_delete(Vector *vector) {
    vector->data[vector->size] = 0;
    vector->size--;
    if (vector->size == (int)(vector->capacity/4)) {
        vector->capacity /= 2;
        vector->data = (int *)realloc(vector->data, vector->capacity * sizeof(int));
    }
}
void vector_free(Vector *vector) {
    free(vector->data);
    vector->size = 0;
    vector->capacity = 0;
}
int main() {
    Vector vector; // declare
    vector_init(&vector); // initialize a vector at the address of vector
    for (int i = 0; i < 10; ++i) {
        vector_add(&vector, i);
    }
    for (size_t i = 0; i < vector.size; ++i) {
        printf("%d", vector_get(&vector,i));
    }
    printf("\n");
    vector_free(&vector);
    return 0;
}