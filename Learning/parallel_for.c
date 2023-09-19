#include <stdio.h>
#include <omp.h>

int main() {
    int N = 100;
    double a[N], b[N], c[N];

    // 初始化a和b
    for (int i = 0; i < N; i++) {
        a[i] = i * 1.0;
        b[i] = (N - i) * 1.0;
    }

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        printf("Thread %d is processing iteration %d\n", omp_get_thread_num(), i);
        c[i] = a[i] + b[i];
    }

    printf("\nResults:\n");
    for (int i = 0; i < N; i++) {
        printf("c[%d] = %f\n", i, c[i]);
    }

    return 0;
}
