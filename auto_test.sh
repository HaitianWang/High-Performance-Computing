#!/bin/bash

# 编译C程序
gcc -o Project_experiment_auto Project_experiment_auto.c -fopenmp -lm

# 实验一
echo "Experiment 1 Results" > experiment1_results.txt
echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment1_results.txt
for i in {1..16}; do
    ./Project_experiment_auto -t $i -s static -c reduction -m default >> experiment1_results.txt
    ./Project_experiment_auto -t $i -s static -c critical -m default >> experiment1_results.txt
done

# 实验二
echo "Experiment 2 Results" > experiment2_results.txt
echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment2_results.txt
declare -a schedules=("static" "dynamic" "guided" "runtime")
for sched in "${schedules[@]}"; do
    for chunk in {1..8}; do
        for threads in 2 4 8; do
            ./Project_experiment_auto -t $threads -s $sched,$chunk -c reduction -m default >> experiment2_results.txt
        done
    done
done

# 实验三
echo "Experiment 3 Results" > experiment3_results.txt
echo "Threads,Schedule,Construct,Cache,Runtime" >> experiment3_results.txt
for threads in 2 4 8; do
    ./Project_experiment_auto -t $threads -s static -c reduction -m default >> experiment3_results.txt
    ./Project_experiment_auto -t $threads -s static -c critical -m default >> experiment3_results.txt
done
