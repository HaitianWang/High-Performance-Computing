#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int main(){
    omp_set_num_threads(3);
    #pragma omp parallel
    {
        // 并行执行的代码
        printf("Thread %d is here.\n", omp_get_thread_num());
        
        #pragma omp single
        {
            // 只有一个线程执行这部分代码
            printf("Only one thread (Thread %d) executes this.\n", omp_get_thread_num());
        }
    }
}