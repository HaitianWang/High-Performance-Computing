/******************************************************************
* Author:        Haitian Wang - 23815631, Long Qing 23829101
* Email:         23815631@student.uwa.edu.au
* 
* Project Name:  Parallel implementation of search based on Fish School Behaviour
* Description:   In first step we create a sequential program to parallel computation
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
Timer *barycentreTimer;                    //  1.   totalTimer?     +    2_Dimensional
double *barycentre;
int currentRound;

// initialize the whole system
void systemInitialize(){
    srand(time(NULL));  // Seed random number generator    
    school = (Fish *)malloc(sizeof(Fish) * N); 
    objectFunctionTimer = (Timer *)malloc(sizeof(Timer) * ROUND); 
    weightTimer = (Timer *)malloc(sizeof(Timer) * ROUND); 
    barycentreTimer = (Timer *)malloc(sizeof(Timer) * ROUND); 
    barycentre = (double *)malloc(sizeof(double) * ROUND); 
    currentRound = 0;
}

// initialize the fish school in the pool
void initializeFish() {
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

// move the fishes is each round: generate 2 random little numbers and add them in coordinates with both x and y of each fish and calculate the objective function currrently in this round.
void move() {
    for(int i = 0; i < N; i++) {
        school[i].x += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].prevObjective = school[i].currentObjective; 
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);    
    }
}

// each fish in fish school eat the food: calculate the current weight for each fish in fish school in this round.
void eat() {
    double maxDiff = -INFINITY;         
    for(int i = 0; i < N; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        if(diff > maxDiff) {          
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

// orient each fish to idea direction based on collective experience - barycentre: calculate the barycentre for each round for all fishes in the fish school
void collectiveExperience() {
    double barycentreNumerator = 0.0;
    double barycentreDenominator = 0.0;
    for(int i = 0; i < N; i++) {
        barycentreNumerator += school[i].currentObjective * school[i].weight;
        barycentreDenominator += school[i].currentObjective;
    }

    if (barycentreDenominator == 0.0) {
        printf("Calculation Error\n");
        exit(1);
    }

    barycentre[currentRound] = barycentreNumerator / barycentreDenominator;
}

// this is the function for optimizing the FSB: operating and start all system.
void optimization() {
    for(int i = 0; i < ROUND; i++) {               //2 . timer  ??
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

// Free the dynamically allocated memory before exiting.
void freeAll(){
    free(school);  
    free(objectFunctionTimer);
    free(weightTimer);
    free(barycentreTimer);
    free(barycentre);
}

int main() {
    systemInitialize();
    initializeFish();
    optimization();
   
                                       // 3.    fwrite（）   write  the data  into  file, code:  ?     
     freeAll();
    return 0;
}







