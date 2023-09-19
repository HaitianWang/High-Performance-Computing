#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define NUM_THREADS 4
#define STATIC_CHUNK 5
#define DYNAMIC_CHUNK 5
#define NUM_LOOPS 20
#define SLEEP_EVERY_N 3

int main() {
    int nStatic1[NUM_LOOPS], nStaticN[NUM_LOOPS];
    int nDynamic1[NUM_LOOPS], nDynamicN[NUM_LOOPS];
    int nGuided[NUM_LOOPS];

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        #pragma omp for schedule(static, 1)
        for (int i = 0; i < NUM_LOOPS; i++) {
            if (i % SLEEP_EVERY_N == 0)
                sleep(0);
            nStatic1[i] = omp_get_thread_num();
        }

        #pragma omp for schedule(static, STATIC_CHUNK)
        for (int i = 0; i < NUM_LOOPS; i++) {
            if (i % SLEEP_EVERY_N == 0)
                sleep(0);
            nStaticN[i] = omp_get_thread_num();
        }

        #pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < NUM_LOOPS; i++) {
            if (i % SLEEP_EVERY_N == 0)
                sleep(0);
            nDynamic1[i] = omp_get_thread_num();
        }

        #pragma omp for schedule(dynamic, DYNAMIC_CHUNK)
        for (int i = 0; i < NUM_LOOPS; i++) {
            if (i % SLEEP_EVERY_N == 0)
                sleep(0);
            nDynamicN[i] = omp_get_thread_num();
        }

        #pragma omp for schedule(guided)
        for (int i = 0; i < NUM_LOOPS; i++) {
            if (i % SLEEP_EVERY_N == 0)
                sleep(0);
            nGuided[i] = omp_get_thread_num();
        }
    }

    // 打印结果
    printf("Static, Chunk 1:\n");
    for (int i = 0; i < NUM_LOOPS; i++) {
        printf("Iteration %d: Thread %d\n", i, nStatic1[i]);
    }

    printf("\nStatic, Chunk %d:\n", STATIC_CHUNK);
    for (int i = 0; i < NUM_LOOPS; i++) {
        printf("Iteration %d: Thread %d\n", i, nStaticN[i]);
    }

    printf("\nDynamic, Chunk 1:\n");
    for (int i = 0; i < NUM_LOOPS; i++) {
        printf("Iteration %d: Thread %d\n", i, nDynamic1[i]);
    }

    printf("\nDynamic, Chunk %d:\n", DYNAMIC_CHUNK);
    for (int i = 0; i < NUM_LOOPS; i++) {
        printf("Iteration %d: Thread %d\n", i, nDynamicN[i]);
    }

    printf("\nGuided:\n");
    for (int i = 0; i < NUM_LOOPS; i++) {
        printf("Iteration %d: Thread %d\n", i, nGuided[i]);
    }

    return 0;
}
