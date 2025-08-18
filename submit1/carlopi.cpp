#include <iostream>
#include <math.h>
#include <pthread.h>
#include <cstdlib>
#include <time.h>

using namespace std;


long long num_tosses;
int num_threads;
long long *counts;


void* montecarlo_worker(void* arg) {
    long id = (long) arg;
    unsigned int seed = time(NULL) ^ id; 

    long long tosses_per_thread = num_tosses / num_threads;
    long long local_count = 0;

    for (long long i = 0; i < tosses_per_thread; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        if (x*x + y*y <= 1.0) {
            local_count++;
        }
    }

    counts[id] = local_count;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <numero_hilos> <numero_tosses>" << endl;
        return 1;
    }

    num_threads = atoi(argv[1]);
    num_tosses = atoll(argv[2]);

    counts = new long long[num_threads];

   
    clock_t start_cpu = clock();
    time_t start_wall = time(NULL);

    pthread_t threads[num_threads];

 
    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, montecarlo_worker, (void*) i);
    }

    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    
    long long total_inside = 0;
    for (int i = 0; i < num_threads; i++) {
        total_inside += counts[i];
    }

    double pi_approx = 4.0 * ((double) total_inside / (double) num_tosses);

    
    clock_t end_cpu = clock();
    time_t end_wall = time(NULL);

    double error = fabs((pi_approx - M_PI) / M_PI);

    cout.precision(15);
    cout << "Tosses: " << num_tosses << endl;
    cout << "Hilos: " << num_threads << endl;
    cout << "Pi aproximado: " << pi_approx << endl;
    cout << "Pi real:       " << M_PI << endl;
    cout << "Error relativo: " << error << endl;
    cout << "Tiempo CPU: "
         << (double)(end_cpu - start_cpu) / CLOCKS_PER_SEC << " segundos" << endl;
    cout << "Tiempo real (wall): "
         << difftime(end_wall, start_wall) << " segundos" << endl;

    delete[] counts;
    return 0;
}
