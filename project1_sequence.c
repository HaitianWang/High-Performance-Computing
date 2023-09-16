/******************************************************************
* Author:        Haitian Wang - 23815631, Long Qing 23829101
* Email:         23815631@student.uwa.edu.au
* 
* Project Name:  Parallel implementation of search based on Fish School Behaviour
* Description:   
* 
* Date Created:  September 16, 2023
* Last Updated:  September 16, 2023
* 
* Institution:   University of Western Australia
* 
* License:       MIT License
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define N 100000000        // Number of fish
#define W 5.0           // Initial weight of fish
#define W_MAX 10.0      // Maximum weight of fish
#define POND_SIZE 200   // Pond size
#define ROUND 10       // Number of rounds

typedef struct {
    double x;
    double y;
    double weight;
    double prevObjective;
    double currentObjective;
} Fish;

typedef struct {
    double time_start;
    double time_end;
    double time_duration;
} Timer;

Fish *school; //pointer to a dynamic array to store the fishes named school
Timer *objectFunctionTimer;
Timer *weightTimer;
Timer *barycentreTimer;
double *barycentre;
int currentRound;

void initializeFish() {
    srand(time(NULL));  // Seed random number generator
    
    school = (Fish *)malloc(sizeof(Fish) * N); 
    objectFunctionTimer = (Timer *)malloc(sizeof(Timer) * ROUND); 
    weightTimer = (Timer *)malloc(sizeof(Timer) * ROUND); 
    barycentreTimer = (Timer *)malloc(sizeof(Timer) * ROUND); 
    barycentre = (double *)malloc(sizeof(double) * ROUND); 
    currentRound = 0;

    if (!school || !objectFunctionTimer || !weightTimer || !barycentreTimer) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    for(int i = 0; i < N; i++) {
        school[i].x = ((double)rand() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].y = ((double)rand() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].weight = W + ((double)rand() / RAND_MAX - 0.5) * 0.1;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }
}

void move() {
    for(int i = 0; i < N; i++) {
        school[i].x += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].prevObjective = school[i].currentObjective; 
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);    
    }
}

void eat() {
    double maxDiff = POND_SIZE;  
    for(int i = 0; i < N; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        if(diff < maxDiff) {
            maxDiff = diff;
        }
    }

    for(int i = 0; i < N; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        school[i].weight += diff / maxDiff;
        if(school[i].weight > W_MAX) {
            school[i].weight = W_MAX;
        }
    }
}

void collectiveExperience() {
    double barycentreNumerator = 0.0;
    double barycentreDenominator = 0.0;
    for(int i = 0; i < N; i++) {
        barycentreNumerator += school[i].currentObjective * school[i].weight;
        barycentreDenominator += school[i].currentObjective;
    }
    barycentre[currentRound] = barycentreNumerator / barycentreDenominator;
}

void optimization() {
    for(int i = 0; i < ROUND; i++) {
        objectFunctionTimer[i].time_start = omp_get_wtime();
        move();
        objectFunctionTimer[i].time_end = omp_get_wtime();
        objectFunctionTimer[i].time_duration = objectFunctionTimer[i].time_end - objectFunctionTimer[i].time_start;
        
        weightTimer[i].time_start = omp_get_wtime();
        eat();
        weightTimer[i].time_end = omp_get_wtime();
        weightTimer[i].time_duration = weightTimer[i].time_end - weightTimer[i].time_start;

        barycentreTimer[i].time_start = omp_get_wtime();
        collectiveExperience();
        barycentreTimer[i].time_end = omp_get_wtime();
        barycentreTimer[i].time_duration = barycentreTimer[i].time_end - barycentreTimer[i].time_start;

        printf("Round %d - Barycentre: %f\n", i+1, barycentre[i]);
        printf("first fish objective: %f\n", school[0].currentObjective);
        printf("objectFunctionTimer: %lf - weightTimer: %lf - barycentreTimer: %lf\n", objectFunctionTimer[i].time_duration, weightTimer[i].time_duration, barycentreTimer[i].time_duration);
        currentRound++;
    }
}


void freeAll(){
    free(school);  // Free the dynamically allocated memory before exiting
    free(objectFunctionTimer);
    free(weightTimer);
    free(barycentreTimer);
    free(barycentre);
}

int main() {
    initializeFish();
    optimization();
    freeAll();
    return 0;
}




