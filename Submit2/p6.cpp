#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    
    int nthreads = atoi(argv[1]);
    long COUNT = atol(argv[2]);
    long long sum = 0;

    omp_set_num_threads(nthreads);

    #pragma omp parallel for private(sum)
    for (long i = 0; i < COUNT; i++) {
        sum += i;   
        printf("Iter=%ld, sum=%lld (thread=%d)\n", i, sum, omp_get_thread_num());
    }
     printf("sum=%lld\n", sum);
    return 0;
}