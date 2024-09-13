#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sched.h> 
#include <sys/time.h>   

#define M 5 // Maximum degree of the B-tree

// B-tree node structure with a mutex lock for each node
struct BTreeNode {
    int num_keys; // Number of keys currently in the node
    int keys[M-1]; // Array of keys
    struct BTreeNode *children[M]; // Array of child pointers
    bool is_leaf; // True if node is a leaf
    pthread_mutex_t lock; // Mutex lock for each node
};

// Function to create a new node
struct BTreeNode *createNode(bool is_leaf) {
    struct BTreeNode *newNode = (struct BTreeNode *)malloc(sizeof(struct BTreeNode));
    if (newNode == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->num_keys = 0;
    newNode->is_leaf = is_leaf;
    for (int i = 0; i < M; i++) {
        newNode->children[i] = NULL;
    }
    pthread_mutex_init(&newNode->lock, NULL); // Initialize node's lock
    return newNode;
}

// Function to split a full child node
void splitChild(struct BTreeNode *parent, int index) {
    struct BTreeNode *child = parent->children[index];
    struct BTreeNode *newNode = createNode(child->is_leaf);
    
    newNode->num_keys = M/2 - 1;
    
    // Move keys and children to the new node
    for (int i = 0; i < M/2 - 1; i++) {
        newNode->keys[i] = child->keys[i + M/2];
    }
    
    if (!child->is_leaf) {
        for (int i = 0; i < M/2; i++) {
            newNode->children[i] = child->children[i + M/2];
        }
    }
    
    child->num_keys = M/2 - 1;
    
    // Shift parent's children to make space for the new node
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    
    parent->children[index + 1] = newNode;
    
    // Shift parent's keys to insert the middle key from the child
    for (int i = parent->num_keys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    
    parent->keys[index] = child->keys[M/2 - 1];
    parent->num_keys++;
}

// Function to insert a key into a non-full node
void insertNonFull(struct BTreeNode *node, int key) {
    pthread_mutex_lock(&node->lock); // Lock current node
    
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        // Insert key into the sorted order
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
        pthread_mutex_unlock(&node->lock); // Unlock the leaf node after insertion
    } else {
        // Find the child to insert the key
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;

        pthread_mutex_lock(&node->children[i]->lock); // Lock child node
        
        if (node->children[i]->num_keys == M - 1) {
            // Split child if it's full
            splitChild(node, i);
            
            // Determine which of the two children is the new one
            if (node->keys[i] < key) {
                i++;
            }
        }
        pthread_mutex_unlock(&node->lock); // Unlock parent node before recursive call
        insertNonFull(node->children[i], key); // Recur to insert in the child
    }
}

// Function to insert a key into the B-tree
void insert(struct BTreeNode **root, int key) {
    if (*root == NULL) {
        *root = createNode(true);
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    struct BTreeNode *node = *root;
    while (true) {
        pthread_mutex_lock(&node->lock);

        if (node->num_keys < M - 1) {
            // Insert key in this non-full node
            int i = node->num_keys - 1;
            while (i >= 0 && node->keys[i] > key) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->num_keys++;
            pthread_mutex_unlock(&node->lock);
            return;
        }

        // Node is full, find the child to descend to
        int i = 0;
        while (i < node->num_keys && key > node->keys[i]) {
            i++;
        }

        if (node->is_leaf) {
            // Cannot insert in a full leaf, need to split
            // (splitting logic here)
            pthread_mutex_unlock(&node->lock);
            return;
        }

        struct BTreeNode *child = node->children[i];
        pthread_mutex_unlock(&node->lock);
        node = child;
    }
}

// Function to traverse and print the B-tree in-order
void traverse(struct BTreeNode *node) {
    if (node != NULL) {
        pthread_mutex_lock(&node->lock); // Lock the current node
        
        int i;
        for (i = 0; i < node->num_keys; i++) {
            pthread_mutex_unlock(&node->lock); // Unlock before descending into child
            traverse(node->children[i]); // Recursively traverse children
            pthread_mutex_lock(&node->lock); // Re-lock the current node after returning from the child
            printf("%d ", node->keys[i]);
        }
        
        pthread_mutex_unlock(&node->lock); // Unlock the current node before visiting last child
        traverse(node->children[i]); // Visit the last child
    }
}

void freeTree(struct BTreeNode* node) {
    if (node != NULL) {
        for (int i = 0; i < node->num_keys; ++i) {
            freeTree(node->children[i]);
        }
        pthread_mutex_destroy(&node->lock);
        free(node);
    }
}



// Main function to test B-tree implementation
int main() {
    struct BTreeNode *root = NULL;

    struct timeval time_before, time_after;
    gettimeofday(&time_before, NULL);
    insert(&root, 10);
    insert(&root, 20);
    insert(&root, 5);
    insert(&root, 6);
    insert(&root, 12);
    insert(&root, 30);
    insert(&root, 11);
    insert(&root, 2);
    insert(&root, 3);
    insert(&root, 4);
    insert(&root, 8);
    insert(&root, 9);
    insert(&root, 13);
    insert(&root, 31);
    insert(&root, 41);
    insert(&root, 23);
    insert(&root, 29);
    insert(&root, 26);
    insert(&root, 15);
    insert(&root, 39);
    insert(&root, 21);
    insert(&root, 38);
    insert(&root, 33);
    insert(&root, 43);
    gettimeofday(&time_after, NULL);
    double elapsed_time_insertion = (time_after.tv_sec - time_before.tv_sec) + 
                            (time_after.tv_usec - time_before.tv_usec) / 1e6; // calculate elapsed time in seconds

    printf("In-order traversal of the B-tree: ");
    traverse(root);
    printf("Time taken to perform all operations: %f\n", elapsed_time_insertion);
    freeTree(root);
    return 0;
}