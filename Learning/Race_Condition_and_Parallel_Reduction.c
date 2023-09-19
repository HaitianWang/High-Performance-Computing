#include <stdio.h>
#include <omp.h>

int roundAA;

int main() {
    int a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int sum = 0;
    roundAA = 0;

    #pragma omp parallel for num_threads(3) schedule(static, 3)  // 使用3个线程
    for (int i = 0; i < 9; i++) {
        sum += a[i];
        printf("now sum: sum = %d, it is the index and the round of %d - %d, belong to thread of %d\n", sum, i, roundAA, omp_get_thread_num());
        roundAA ++;
    }

    printf("Without reduction: sum = %d\n", sum);

    return 0;
}
