#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define NUM_FISH 200000000    // Number of fish
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
double time_reduction = 0.0;

double *barycentre;
int currentRound;
unsigned int *rand_state;  // Thread local state for random number generation
//#pragma omp threadprivate(rand_state)

int rand_threadsafe() {
    int tid = omp_get_thread_num();
    rand_state[tid] = (rand_state[tid] * 11035u + 10086u) % ((unsigned)RAND_MAX + 1);
    return (rand_state[tid] % ((unsigned)RAND_MAX + 1));
}

// initialize the whole system
void systemInitialize(){

    srand(time(NULL));
    int num_threads = omp_get_max_threads(); // Get the number of threads
    rand_state = (unsigned int *)malloc(sizeof(unsigned int) * num_threads);

    if (!rand_state) {
        printf("Memory allocation for rand_state failed!\n");
        exit(1);
    }

    // Seed the random state for each thread
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        rand_state[tid] = rand();
    }

    school = (Fish *)malloc(sizeof(Fish) * NUM_FISH);  //   10000num  of  fish  512 f   mish  atch    size  cache  60      static(4)
    performanceTimer = (Timer *)malloc(sizeof(Timer) * TIMES);
    barycentre = (double *)malloc(sizeof(double) * ROUND);
    currentRound = 0;

    if(!school || !performanceTimer || !barycentre) {
        printf("Memory allocation failed!\n");
        exit(1);
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
void eat() {

    double startTime = omp_get_wtime();
    
    double maxDiff = -INFINITY;
    #pragma omp parallel for 
    for(int i = 0; i < NUM_FISH; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
         #pragma omp critical
        if(diff > maxDiff) {
            maxDiff = diff;
        }
    }
    double endTime = omp_get_wtime();
    time_reduction= endTime - startTime;

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
    performanceTimer[0].time_start = omp_get_wtime();
    for(int i = 0; i < ROUND; i++) {
        move();
        eat();
        collectiveExperience();
        currentRound++;
    }
     performanceTimer[0].time_end = omp_get_wtime();
     performanceTimer[0].time_duration = performanceTimer[0].time_end - performanceTimer[0].time_start;
    printf("%.1lf\t",  performanceTimer[0].time_duration);
   // return  performanceTimer[0].time_duration;
}

// Free the dynamically allocated memory before exiting.
void freeAll(){
    free(school);
    free(performanceTimer);
    free(barycentre);
}

int main() {
    printf("Num_Threads\ttotalTime\tpartTime\n");
    for (int num_threads = 1; num_threads <= 16; num_threads++) {
        omp_set_num_threads(num_threads);
        printf("%d\t", num_threads);
        systemInitialize();
        initializeFish();
        optimization();
        printf("%.1lf\n", time_reduction);
        freeAll();
    }
    return 0;
}