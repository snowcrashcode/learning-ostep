#!/bin/bash

# Number of iterations for benchmark
iterations=100

# Loop to run the context-switch program multiple times
for ((i=0; i<$iterations; i++))
do
    ./context-switch
done