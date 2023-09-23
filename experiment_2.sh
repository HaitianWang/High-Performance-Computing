#!/bin/bash

gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

declare -a schedules=("static" "dynamic" "guided" "runtime")
for sched in "${schedules[@]}"; do
    for chunk in {1..8}; do
        for threads in 2 4 8; do
            filename="experiment2_threads_${threads}_schedule_${sched}_chunk_${chunk}_construct_reduction.csv"
            ./Project_experiment_auto -t $threads -s $sched,$chunk -c reduction -m default -f $filename
        done
    done
done
