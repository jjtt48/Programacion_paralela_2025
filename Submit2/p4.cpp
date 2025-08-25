#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {

    int nthreads = atoi(argv[1]);
    omp_set_num_threads(nthreads);
    #pragma omp parallel for
        for (int i = 0; i < 16; i++) {
            printf("Hello from thread nimbre: %d Iteration: %d \n", omp_get_thread_num(),i);
        }
    printf("\n Goodbye -PUJPP- Exiting Program \n");
    return 0;
}
