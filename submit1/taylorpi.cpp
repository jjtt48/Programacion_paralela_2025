#include <iostream>
#include <math.h>
#include <time.h>   
#include <cstdlib> 

using namespace std;

int main(int argc, char* argv[]) {
    
    long long upper_bound = atoll(argv[1]);

    
    clock_t start_cpu = clock();
    time_t start_wall = time(NULL);

    double pi_approx = 0.0;
    double sign = 1.0;

    for (long long k = 0; k < upper_bound; k++) {
        pi_approx += sign / (2.0 * k + 1.0);
        sign = -sign; 
    }

    pi_approx *= 4.0;

    
    clock_t end_cpu = clock();
    time_t end_wall = time(NULL);

    double error = fabs((pi_approx - M_PI) / M_PI);

    cout.precision(15);
    cout << "Iteraciones: " << upper_bound << endl;
    cout << "Pi aproximado: " << pi_approx << endl;
    cout << "Pi real:       " << M_PI << endl;
    cout << "Error relativo: " << error << endl;
    cout << "Tiempo CPU: " 
         << (double)(end_cpu - start_cpu) / CLOCKS_PER_SEC << " segundos" << endl;
    cout << "Tiempo real (wall): " 
         << difftime(end_wall, start_wall) << " segundos" << endl;

    return 0;
}
