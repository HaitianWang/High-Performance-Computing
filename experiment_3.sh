#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

# 实验三
for threads in 2 4 8; do
    output_file="experiment3_threads_${threads}_schedule_static_construct_reduction_cache_default.csv"
    echo "Threads,Schedule,Construct,Cache,Runtime" > $output_file
    ./Project_experiment_auto -t $threads -s static -c reduction -m default >> $output_file

    output_file="experiment3_threads_${threads}_schedule_static_construct_critical_cache_default.csv"
    echo "Threads,Schedule,Construct,Cache,Runtime" > $output_file
    ./Project_experiment_auto -t $threads -s static -c critical -m default >> $output_file
done
