#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <string.h>
#include <ctype.h>


typedef enum {
    STATIC = 0,
    DYNAMIC,
    GUIDED,
    RUNTIME
} ScheduleType;

typedef enum {
    REDUCTION = 0,
    CRITICAL
} ConstructType;

typedef struct {
    int num_fish;
    double weight;
    double w_max;
    int pond_size;
    int total_rounds;
    int round;
    int times;
    int num_threads;
    ScheduleType schedule;
    ConstructType construct;
    int chunk_size;
    char cache[256]; 
    char filename[256];  
} Config;

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

static unsigned int* rand_state;

int rand_threadsafe(int tid) {
    rand_state[tid] = (rand_state[tid] * 11035u + 10086u) % ((unsigned)RAND_MAX + 1);
    return (rand_state[tid] % ((unsigned)RAND_MAX + 1));
}

const char* scheduleTypeToStr(ScheduleType type) {
    switch(type) {
        case STATIC: return "static";
        case DYNAMIC: return "dynamic";
        case GUIDED: return "guided";
        case RUNTIME: return "runtime";
        default: return "unknown";
    }
}

void systemInitialize(const Config* config) {
    srand(time(NULL));
    omp_set_num_threads(config->num_threads);
    rand_state = (unsigned int*)malloc(sizeof(unsigned int) * config->num_threads);
    if (!rand_state) {
        printf("Memory allocation failed for rand_state!\n");
        exit(1);
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        rand_state[tid] = rand();
    }
}

Fish* initializeFish(const Config* config) {
    Fish* school = (Fish *)malloc(sizeof(Fish) * config->num_fish);
    if (!school) {
        printf("Memory allocation failed for school!\n");
        exit(1);
    }

    #pragma omp parallel for
    for(int i = 0; i < config->num_fish; i++) {
        int tid = omp_get_thread_num();
        school[i].x = ((double)rand_threadsafe(tid) / RAND_MAX - 0.5) * config->pond_size;
        school[i].y = ((double)rand_threadsafe(tid) / RAND_MAX - 0.5) * config->pond_size;
        school[i].weight = config->weight + ((double)rand_threadsafe(tid) / RAND_MAX - 0.5) * 0.1;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }

    return school;
}

void move(Fish* school, const Config* config) {
    #pragma omp parallel for
    for(int i = 0; i < config->num_fish; i++) {
        int tid = omp_get_thread_num();
        school[i].x += (double)rand_threadsafe(tid) / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand_threadsafe(tid) / RAND_MAX * 0.2 - 0.1;
        school[i].prevObjective = school[i].currentObjective;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }
}


void eat(Fish* school, const Config* config) {
    double maxDiff = -INFINITY;

    if (config->construct == REDUCTION) {
        #pragma omp parallel for reduction(max: maxDiff)
        for(int i = 0; i < config->num_fish; i++) {
            double diff = school[i].prevObjective - school[i].currentObjective;
            if(diff > maxDiff) {
                maxDiff = diff;
            }
        }
    } else if (config->construct == CRITICAL) {
        #pragma omp parallel for
        for(int i = 0; i < config->num_fish; i++) {
            double diff = school[i].prevObjective - school[i].currentObjective;
            #pragma omp critical
            if(diff > maxDiff) {
                maxDiff = diff;
            }
        }
    }

    #pragma omp parallel for
    for(int i = 0; i < config->num_fish; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        school[i].weight += diff / maxDiff;
        if(school[i].weight > config->w_max) {
            school[i].weight = config->w_max;
        }
    }
}

void collectiveExperience(Fish* school, double* barycentre, const Config* config) {
    double barycentreNumerator = 0.0;
    double barycentreDenominator = 0.0;
    #pragma omp parallel for reduction(+:barycentreNumerator, barycentreDenominator)
    for(int i = 0; i < config->num_fish; i++) {
        barycentreNumerator += school[i].currentObjective * school[i].weight;
        barycentreDenominator += school[i].currentObjective;
    }

    if (barycentreDenominator == 0.0) {
        printf("Calculation Error\n");
        exit(1);
    }

    barycentre[config->round] = barycentreNumerator / barycentreDenominator;
}

void optimization(Fish* school, double* barycentre, const Config* config) {
    for(int r = 0; r < config -> total_rounds; r++) {
        move(school, config);
        eat(school, config);
        collectiveExperience(school, barycentre, config);
    }
}

void experiment(Fish* school, Timer* performanceTimer, double* barycentre, Config* config, const char* filename) {
    FILE *file = fopen(filename, "a");  
    if(!file) {
        perror("Failed to open the output file");
        exit(1);
    }

    double totalExperimentTime = 0;
    for (int i = 0; i < config->times; i++) {
        config->round = 0;
        performanceTimer[i].time_start = omp_get_wtime();
        optimization(school, barycentre, config);
        performanceTimer[i].time_end = omp_get_wtime();
        performanceTimer[i].time_duration = performanceTimer[i].time_end - performanceTimer[i].time_start;
        totalExperimentTime +=  performanceTimer[i].time_duration;
    }

    double averageExperimentTime = totalExperimentTime / config->times;
    fprintf(file, "%d,%s,%d,%s,%s,%lf\n", 
        config->num_threads, 
        scheduleTypeToStr(config->schedule),
        (config->schedule == RUNTIME) ? 0 : str(config->chunk_size), 
        (config->construct == REDUCTION) ? "reduction" : "critical", 
        "default",
        averageExperimentTime
    );

    fclose(file);
}


int main(int argc, char* argv[]) {

    Config config = {
        .num_fish = 5000000,
        .weight = 5.0,
        .w_max = 10.0,
        .pond_size = 200,
        .round = 0,
        .total_rounds = 10,
        .times = 5,
        .num_threads = 8,
        .schedule = STATIC,
        .construct = REDUCTION,
        .chunk_size = 1,
        .cache = "default",
        .filename = {0}
    };

    strcpy(config.filename, "default_output.csv");

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-t") == 0) {
            config.num_threads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0) {
            i++;
            if (strcmp(argv[i], "static") == 0) {
                config.schedule = STATIC;
                if (i + 1 < argc && isdigit(argv[i + 1][0])) {
                    config.chunk_size = atoi(argv[++i]);
                } else {
                    config.chunk_size = 0;
                }
            } else if (strcmp(argv[i], "dynamic") == 0) {
                config.schedule = DYNAMIC;
                if (i + 1 < argc && isdigit(argv[i + 1][0])) {
                    config.chunk_size = atoi(argv[++i]);
                } else {
                    config.chunk_size = 0;
                }
            } else if (strcmp(argv[i], "guided") == 0) {
                config.schedule = GUIDED;
                if (i + 1 < argc && isdigit(argv[i + 1][0])) {
                    config.chunk_size = atoi(argv[++i]);
                } else {
                    config.chunk_size = 0;
                }
            } else if (strcmp(argv[i], "runtime") == 0) {
                config.schedule = RUNTIME;
            }
        } else if (strcmp(argv[i], "-c") == 0) {
            i++;
            if (strcmp(argv[i], "reduction") == 0) {
                config.construct = REDUCTION;
            } else if (strcmp(argv[i], "critical") == 0) {
                config.construct = CRITICAL;
            }
        }
        else if (strcmp(argv[i], "-f") == 0) {
            strcpy(config.filename, argv[++i]);
        }

    }

    // 设置调度策略
    switch (config.schedule) {
        case STATIC:
            omp_set_schedule(omp_sched_static, config.chunk_size);
            break;
        case DYNAMIC:
            omp_set_schedule(omp_sched_dynamic, config.chunk_size);
            break;
        case GUIDED:
            omp_set_schedule(omp_sched_guided, config.chunk_size);
            break;
        case RUNTIME:
            omp_set_schedule(omp_sched_auto, 0);
            break;
    }

    systemInitialize(&config);
    Fish* school = initializeFish(&config);

    Timer* performanceTimer = (Timer*)malloc(sizeof(Timer) * config.times);
    double* barycentre = (double*)malloc(sizeof(double) * config.round);

    if (!performanceTimer || !barycentre) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    experiment(school, performanceTimer, barycentre, &config, config.filename);


    free(barycentre);
    free(performanceTimer);
    free(school);
    free(rand_state);

    return 0;
}
