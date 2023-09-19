#include <stdio.h>
#include <omp.h>

int main() {
    printf("With ordered directive:\n");
    #pragma omp parallel for schedule(static) ordered
    for (int i = 0; i < 100; i++) {
        if (i % 2 == 0) {
            #pragma omp ordered
            printf("Ordered: test() iteration %d\n", i);
        }
    }

    printf("\nWithout ordered directive:\n");
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < 100; i++) {
        if (i % 2 == 0) {
            printf("Unordered: test() iteration %d\n", i);
        }
    }

    return 0;
}
