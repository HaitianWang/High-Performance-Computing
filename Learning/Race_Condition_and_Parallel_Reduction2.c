#include <stdio.h>
#include <omp.h>

int main() {
    int a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int sum = 0;

    #pragma omp parallel num_threads(3)  // 使用3个线程并添加reduction子句
    #pragma omp for reduction(+:sum) schedule(static,3)
    for (int i = 0; i < 9; i++) {
        sum += a[i];
        printf("now sum: sum = %d, it is the round of %d, belong to thread of %d\n", sum, i, omp_get_thread_num());
    }

    printf("With reduction: sum = %d\n", sum);

    return 0;
}
