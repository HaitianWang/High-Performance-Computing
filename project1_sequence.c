/******************************************************************
* Author:        Haitian Wang - 23815631, Long Qing 23829101
* Email:         23815631@student.uwa.edu.au
* 
* Project Name:  Parallel implemetation of search based on Fish School Behaviour
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


#define N 100           // Number of fish
#define W 5.0           // Initial weight of fish
#define W_MAX 10.0      // Maximum weight of fish
#define POND_SIZE 200   // Pond size
#define ROUND 100        // Round


typedef struct {
    double x;
    double y;
    double weight;
} Fish;

Fish *school;

void initializeFish() {
    school = (Fish *)malloc(sizeof(Fish) * N);  // Dynamic allocation for fish
    if (!school) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    for(int i = 0; i < N; i++) {
        school[i].x = ((double)rand() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].y = ((double)rand() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].weight = W;
    }
}

double individualObjectiveFunction(Fish fish) {
    return sqrt(fish.x * fish.x + fish.y * fish.y);
}

void move() {
    for(int i = 0; i < N; i++) {
        school[i].x += (double)rand() / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand() / RAND_MAX * 0.2 - 0.1;
    }
}

void eat() {
    double prevObjective[N];
    for(int i = 0; i < N; i++) {
        prevObjective[i] = individualObjectiveFunction(school[i]);
    }
    
    // Move before eating (according to your problem specification)
    move();

    double maxDiff = 0.0;
    for(int i = 0; i < N; i++) {
        double currentObjective = individualObjectiveFunction(school[i]);
        double diff = prevObjective[i] - currentObjective;
        if(fabs(diff) > fabs(maxDiff)) {
            maxDiff = diff;
        }
    }

    for(int i = 0; i < N; i++) {
        double currentObjective = individualObjectiveFunction(school[i]);
        double diff = prevObjective[i] - currentObjective;
        school[i].weight += diff / maxDiff;
        if(school[i].weight > W_MAX) {
            school[i].weight = W_MAX;
        }
    }
}


double collectiveExperience() {
    double barycentreNumerator = 0.0;
    double barycentreDenominator = 0.0;
    for(int i = 0; i < N; i++) {
        double objective = individualObjectiveFunction(school[i]);
        barycentreNumerator += objective * school[i].weight;
        barycentreDenominator += objective;
    }
    return barycentreNumerator / barycentreDenominator;
}

void optimization(){

        for(int i = 0; i < ROUND; i++) {
        eat();
        printf("Round %d - Barycentre: %f\n", i+1, collectiveExperience());
    }
}

int main() {
    srand(time(NULL)); // Seed random number generator

    initializeFish();

    for(int i = 0; i < ROUND; i++) {
        eat();
        printf("Round %d - Barycentre: %f\n", i+1, collectiveExperience());
    }
    optimization();
    return 0;
}





