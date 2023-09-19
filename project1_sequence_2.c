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

#define NUM_FISH 10000000        // Number of fish
#define WEIGHT 5.0           // Initial weight of fish
#define W_MAX 10.0      // Maximum weight of fish
#define POND_SIZE 200   // Pond size
#define ROUND 10       // Number of rounds
#define TIMES 5         // number of experiments

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
Timer *performanceTimer;
double *barycentre;
int currentRound;
int currentExperiment;

// initialize the whole system
void systemInitialize(){
    srand(time(NULL));  // Seed random number generator    
    school = (Fish *)malloc(sizeof(Fish) * NUM_FISH); 
    performanceTimer = (Timer *)malloc(sizeof(Timer) * TIMES); 
    barycentre = (double *)malloc(sizeof(double) * ROUND); 
    currentRound = 0;
    currentExperiment = 0;

    if (!school || !performanceTimer || !barycentre) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

}

// initialize the fish school in the pool
void initializeFish() {
    for(int i = 0; i < NUM_FISH; i++) {
        school[i].x = ((double)rand() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].y = ((double)rand() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].weight = WEIGHT + ((double)rand() / RAND_MAX - 0.5) * 0.1;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }
}

// move the fishes is each round: generate 2 random little numbers and add them in coordinates with both x and y of each fish and calculate the objective function currrently in this round.
void move() {
    for(int i = 0; i < NUM_FISH; i++) {
        school[i].x += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].prevObjective = school[i].currentObjective; 
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);    
    }
}

// each fish in fish school eat the food: calculate the current weight for each fish in fish school in this round.
void eat() {
    double maxDiff = -INFINITY;         
    for(int i = 0; i < NUM_FISH; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        if(diff > maxDiff) {          
            maxDiff = diff;            
        }
    }

    for(int i = 0; i < NUM_FISH; i++) {
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
    for(int i = 0; i < NUM_FISH; i++) {
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
    for(int i = 0; i < ROUND; i++) {               
        move();
        eat();
        collectiveExperience();

        printf("Round %d - Barycentre: %f\n", i+1, barycentre[i]);
        printf("first fish objective: %f\n", school[0].currentObjective);
        currentRound++;
    }
}


void experiment(){

    for (int i = 0; i < TIMES; i++){
        performanceTimer[i].time_start = omp_get_wtime();
        optimization();
        performanceTimer[i].time_end = omp_get_wtime();
        performanceTimer[i].time_duration = performanceTimer[i].time_end - performanceTimer[i].time_start;
        printf("performanceTimer: %lf\n", performanceTimer[i].time_duration);
        currentExperiment++;
    }

}


// Free the dynamically allocated memory before exiting.
void freeAll(){
    free(school);  
    free(performanceTimer);
    free(barycentre);
}

int main() {
    systemInitialize();
    initializeFish();
    experiment(); 
    freeAll();
    return 0;
}







