#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

# 实验一
for i in {1..16}; do
    output_file="experiment1_threads_${i}_schedule_static_construct_reduction_cache_default.csv"
    echo "Threads,Schedule,Construct,Cache,Runtime" > $output_file
    ./Project_experiment_auto -t $i -s static -c reduction -m default >> $output_file

    output_file="experiment1_threads_${i}_schedule_static_construct_critical_cache_default.csv"
    echo "Threads,Schedule,Construct,Cache,Runtime" > $output_file
    ./Project_experiment_auto -t $i -s static -c critical -m default >> $output_file
done
