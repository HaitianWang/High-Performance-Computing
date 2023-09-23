#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

# 实验二
declare -a schedules=("static" "dynamic" "guided" "runtime")
for sched in "${schedules[@]}"; do
    for chunk in {1..8}; do
        for threads in 2 4 8; do
            if [[ "$sched" == "runtime" ]]; then
                output_file="experiment2_threads_${threads}_schedule_${sched}_construct_reduction_cache_default.csv"
                echo "Threads,Schedule,Construct,Cache,Runtime" > $output_file
                ./Project_experiment_auto -t $threads -s $sched -c reduction -m default >> $output_file
            else
                output_file="experiment2_threads_${threads}_schedule_${sched}_${chunk}_construct_reduction_cache_default.csv"
                echo "Threads,Schedule,Construct,Cache,Runtime" > $output_file
                ./Project_experiment_auto -t $threads -s $sched,$chunk -c reduction -m default >> $output_file
            fi
        done
    done
done
