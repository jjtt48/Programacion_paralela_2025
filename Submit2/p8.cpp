#include <stdio.h>
#include <omp.h>
#include <time.h>

long long num_steps = 1000000000;  
double step;

int main(int argc, char* argv[]) {
    double x, pi, sum = 0.0;
    int i;
    
    step = 1.0 / (double) num_steps;
    for (i = 0; i < num_steps; i++) {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    pi = sum * step;
    
    printf("The value of PI is %15.12f\n", pi);
    

    return 0;
}
