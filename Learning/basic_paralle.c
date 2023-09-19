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

    #pragma omp parallel
    {
        int id, i, Nthrds, istart, iend;
        id = omp_get_thread_num();
        Nthrds = omp_get_num_threads();
        istart = id * N / Nthrds;
        iend = (id + 1) * N / Nthrds;
        if (id == Nthrds - 1) iend = N;

        printf("Thread %d is processing iterations from %d to %d\n", id, istart, iend-1);

        for (i = istart; i < iend; i++) {
            c[i] = a[i] + b[i];
        }
    }

    printf("\nResults:\n");
    for (int i = 0; i < N; i++) {
        printf("c[%d] = %f\n", i, c[i]);
    }

    return 0;
}
