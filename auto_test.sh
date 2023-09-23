#!/bin/bash

# 编译C程序
gcc your_program.c -o your_program -fopenmp

# 实验一
for i in {1..16}; do
    ./your_program -t $i -s static -c reduction -m default >> experiment1_results.txt
    ./your_program -t $i -s static -c critical -m default >> experiment1_results.txt
done

# 实验二
declare -a schedules=("static" "dynamic" "guided" "runtime")
for sched in "${schedules[@]}"; do
    for chunk in {1..8}; do
        for threads in 2 4 8; do
            ./your_program -t $threads -s $sched -c reduction -m default >> experiment2_results.txt
        done
    done
done

# 实验三
for threads in 2 4 8; do
    ./your_program -t $threads -s static -c reduction -m default >> experiment3_results.txt
    ./your_program -t $threads -s static -c critical -m default >> experiment3_results.txt
done

# 实验四
# 在这里添加你的缓存管理方式
for threads in 2 4 8; do
    ./your_program -t $threads -s static -c reduction -m cache_mode1 >> experiment4_results.txt
    # 添加其他缓存管理方式...
done

