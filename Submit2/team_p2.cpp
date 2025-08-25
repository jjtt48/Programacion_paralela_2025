#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    
    int nthreads = atoi(argv[1]);   
    omp_set_num_threads(nthreads);  

    #pragma omp parallel
    {
        printf("Hello ");
    }

    printf("\nGoodBye -PUJPP- Exiting Program\n");
    return 0;
}
