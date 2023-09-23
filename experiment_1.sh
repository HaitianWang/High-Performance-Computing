#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

filename="experiment1_results.csv"

# Write headers
echo "Threads,Schedule,Chunk,Construct,Cache,Runtime" > $filename

# 定义constructs数组
declare -a constructs=("reduction" "critical")

# 遍历1到16的线程数
for i in {1..16}; do
    # 遍历每个construct
    for constr in "${constructs[@]}"; do
        ./Project_experiment_auto -t $i -s static 50 -c $constr -f $filename
    done
done
