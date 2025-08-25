#include <stdio.h>
#include <omp.h>
int main() {
    #pragma omp parallel
    printf("Hello");
    printf("\nGoodBye -PUJPP- Exiting Program\n");
    return 0;
}