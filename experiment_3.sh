#!/bin/bash

gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

for threads in 2 4 8; do
    filename="experiment3_threads_${threads}_schedule_static_construct_reduction.csv"
    ./Project_experiment_auto -t $threads -s static -c reduction -m default -f $filename

    filename="experiment3_threads_${threads}_schedule_static_construct_critical.csv"
    ./Project_experiment_auto -t $threads -s static -c critical -m default -f $filename
done
