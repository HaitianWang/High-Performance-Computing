#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

filename_reduction="experiment1_schedule_static_construct_reduction.csv"
filename_critical="experiment1_schedule_static_construct_critical.csv"

# Write headers
echo "Threads,Average Performance Time" > $filename_reduction
echo "Threads,Average Performance Time" > $filename_critical

for i in {1..16}; do
    ./Project_experiment_auto -t $i -s static -c reduction -m default -f $filename_reduction
    ./Project_experiment_auto -t $i -s static -c critical -m default -f $filename_critical
done
