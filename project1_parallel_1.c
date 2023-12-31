/******************************************************************
* Author:        Haitian Wang - 23815631, Long Qing 23829101
* Email:         23815631@student.uwa.edu.au
* 
* Project Name:  Parallel implementation of search based on Fish School Behaviour
* Description:   In second step we create a experiments program to parallel computation 
                 and optimization the performance of whole program
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

#define NUM_FISH 10000000       // Number of fish
#define WEIGHT 5.0           // Initial weight of fish
#define W_MAX 10.0      // Maximum weight of fish
#define POND_SIZE 200   // Pond size
#define ROUND 10       // Number of rounds
#define TIMES 5         // number of experiments
#define NUM_THREADS 4   // Number of THREADS in this process

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
//double *experimentTime;
double averageExperimentTime;
// unsigned int rand_seeds[NUM_THREADS];
static unsigned int rand_state[NUM_THREADS];  // Thread local state for random number generation
#pragma omp threadprivate(rand_state)


int rand_threadsafe() {
    int tid = omp_get_thread_num();
    rand_state[tid] = (rand_state[tid] * 11035u + 10086u) % ((unsigned)RAND_MAX + 1);
    return (rand_state[tid] % ((unsigned)RAND_MAX + 1));
}

// initialize the whole system
void systemInitialize(){

    srand(time(NULL));
    omp_set_num_threads(NUM_THREADS);  
    school = (Fish *)malloc(sizeof(Fish) * NUM_FISH); 
    performanceTimer = (Timer *)malloc(sizeof(Timer) * TIMES); 
    barycentre = (double *)malloc(sizeof(double) * ROUND); 
    currentRound = 0;
    currentExperiment = 0;
    averageExperimentTime = 0;
    //experimentTime = (double *)malloc(sizeof(double) * TIMES); 

    if(!school || !performanceTimer || !barycentre) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    
    // Seed the random state for each thread
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        rand_state[tid] = rand();
    }
}

// initialize the fish school in the pool
void initializeFish() {
    #pragma omp parallel for
    for(int i = 0; i < NUM_FISH; i++) {
        int tid = omp_get_thread_num();
        school[i].x = ((double)rand_threadsafe() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].y = ((double)rand_threadsafe() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].weight = WEIGHT + ((double)rand_threadsafe() / RAND_MAX - 0.5) * 0.1;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }
}

// move the fishes is each round: generate 2 random little numbers and add them in coordinates with both x and y of each fish and calculate the objective function currrently in this round.
void move() {
    #pragma omp parallel for
    for(int i = 0; i < NUM_FISH; i++) {
        int tid = omp_get_thread_num();
        school[i].x += (double)rand_threadsafe() / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand_threadsafe() / RAND_MAX * 0.2 - 0.1;
        school[i].prevObjective = school[i].currentObjective; 
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);    
    }
}

// each fish in fish school eat the food: calculate the current weight for each fish in fish school in this round.
void eat_reduction() {
    double maxDiff = -INFINITY;  
    #pragma omp parallel for reduction(max: maxDiff)       
    for(int i = 0; i < NUM_FISH; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        if(diff > maxDiff) {          
            maxDiff = diff;            
        }
    }

    #pragma omp parallel for
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
    #pragma omp parallel for reduction(+:barycentreNumerator, barycentreDenominator)
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
        eat_reduction();
        collectiveExperience();
        printf("Round %d - Barycentre: %f\n", i+1, barycentre[i]);
        printf("first fish objective: %f\n", school[0].currentObjective);
        currentRound++;
    }
}


void experiment(){

    double totalExperimentTime = 0;
    for (int i = 0; i < TIMES; i++){
        performanceTimer[i].time_start = omp_get_wtime();
        optimization();
        performanceTimer[i].time_end = omp_get_wtime();
        performanceTimer[i].time_duration = performanceTimer[i].time_end - performanceTimer[i].time_start;
        printf("performance Time: %lf in the Times of %d\n", performanceTimer[i].time_duration, i);
        currentExperiment++;
        totalExperimentTime +=  performanceTimer[i].time_duration;
    }
    
    averageExperimentTime = totalExperimentTime/TIMES;
    printf("The final average performance time is: %lf\n", averageExperimentTime);
}


// Free the dynamically allocated memory before exiting.
void freeAll(){
    free(school);  
    free(performanceTimer);
    free(barycentre);
    //free(rand_seeds);
}

int main() {
    printf("1");
    systemInitialize();
    printf("2");
    initializeFish();
    printf("3");
    experiment(); 
    printf("4");
    freeAll();
    return 1;
}







