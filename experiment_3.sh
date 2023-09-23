#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

filename="experiment3_results.csv"

# Write headers
echo "Threads,Schedule,Chunk,Construct,Cache,Runtime" > $filename

# 定义constructs数组
declare -a constructs=("reduction" "critical")

# x_1线程数的取值
declare -a thread_values=(2 4 8)

# 遍历每个线程数
for threads in "${thread_values[@]}"; do
    # 遍历每个construct
    for constr in "${constructs[@]}"; do
        ./Project_experiment_auto -t $threads -s static -c $constr -f $filename
    done
done
