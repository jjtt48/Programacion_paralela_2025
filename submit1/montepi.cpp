#include <iostream>
#include <math.h>
#include <time.h>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <numero_tosses>" << endl;
        return 1;
    }

    long long num_tosses = atoll(argv[1]);
    if (num_tosses <= 0) {
        cerr << "El numero de tosses debe ser > 0" << endl;
        return 1;
    }

    
    clock_t start_cpu = clock();
    time_t start_wall = time(NULL);

    srand(time(NULL)); 
    long long inside = 0;

    for (long long i = 0; i < num_tosses; i++) {
        double x = (double)rand() / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand() / RAND_MAX * 2.0 - 1.0;
        if (x*x + y*y <= 1.0) inside++;
    }

    double pi_approx = 4.0 * (double)inside / (double)num_tosses;

    
    clock_t end_cpu = clock();
    time_t end_wall = time(NULL);

    double error = fabs((pi_approx - M_PI) / M_PI);

    cout.precision(15);
    cout << "Tosses: " << num_tosses << endl;
    cout << "Pi aproximado: " << pi_approx << endl;
    cout << "Pi real:       " << M_PI << endl;
    cout << "Error relativo: " << error << endl;
    cout << "Tiempo CPU: "
         << (double)(end_cpu - start_cpu) / CLOCKS_PER_SEC << " segundos" << endl;
    cout << "Tiempo real (wall): "
         << difftime(end_wall, start_wall) << " segundos" << endl;

    return 0;
}
