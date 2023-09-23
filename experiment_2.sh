#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

filename="experiment2_results.csv"

# Write headers
echo "Threads,Schedule,Chunk,Construct,Runtime" > $filename

# 定义调度策略数组
declare -a schedules=("static" "dynamic" "guided" "runtime")

# x_1线程数的取值
declare -a thread_values=(2 4 8)

# 遍历每个线程数
for threads in "${thread_values[@]}"; do
    # 遍历每个调度策略
    for sched in "${schedules[@]}"; do
        # 对于runtime调度策略，不考虑chunk
        if [ "$sched" == "runtime" ]; then
            ./Project_experiment_auto -t $threads -s $sched -c reduction -f $filename
        else
            # 对于static, dynamic, guided调度策略，考虑chunk值
            for n in {1..12}; do
                chunk=$((2**$n))
                ./Project_experiment_auto -t $threads -s $sched -c reduction -f $filename
            done
        fi
    done
done
