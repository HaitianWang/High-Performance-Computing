#!/bin/bash

gcc -fopenmp experiment.c -o experiment

# experimental 1
for threads in {1..16}; do
    export OMP_NUM_THREADS=$threads
    echo "Number of threads: $threads" >> results.txt
    ./experiment >> results.txt
    echo "--------------------" >> results.txt
done

# experimental 2
declare -a schedules=("static" "dynamic" "guided" "runtime")
for schedule in "${schedules[@]}"; do
    for threads in 2 4 8; do
        for chunk in {1..8}; do
            export OMP_NUM_THREADS=$threads
            export OMP_SCHEDULE="$schedule,$chunk"
            echo "Number of threads: $threads, Schedule: $schedule, Chunk size: $chunk" >> results.txt
            ./experiment >> results.txt
            echo "--------------------" >> results.txt
        done
    done
done

