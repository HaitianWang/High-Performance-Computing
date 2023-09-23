#!/bin/bash

# 编译C程序
gcc your_program.c -o your_program -fopenmp

# 实验一
echo "Experiment 1 Results" > experiment1_results.txt
echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment1_results.txt
for i in {1..16}; do
    ./your_program -t $i -s static -c reduction -m default >> experiment1_results.txt
    ./your_program -t $i -s static -c critical -m default >> experiment1_results.txt
done

# 实验二
echo "Experiment 2 Results" > experiment2_results.txt
echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment2_results.txt
declare -a schedules=("static" "dynamic" "guided" "runtime")
for sched in "${schedules[@]}"; do
    for chunk in {1..8}; do
        for threads in 2 4 8; do
            ./your_program -t $threads -s $sched,$chunk -c reduction -m default >> experiment2_results.txt
        done
    done
done

# 实验三
echo "Experiment 3 Results" > experiment3_results.txt
echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment3_results.txt
for threads in 2 4 8; do
    ./your_program -t $threads -s static -c reduction -m default >> experiment3_results.txt
    ./your_program -t $threads -s static -c critical -m default >> experiment3_results.txt
done

# # 实验四
# echo "Experiment 4 Results" > experiment4_results.txt
# echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment4_results.txt
# # 在这里添加你的缓存管理方式
# declare -a caches=("cache_mode1" "cache_mode2" "cache_mode3") # 你可以按需求继续添加
# for cache in "${caches[@]}"; do
#     for threads in 2 4 8; do
#         ./your_program -t $threads -s static -c reduction -m $cache >> experiment4_results.txt
#     done
# done
