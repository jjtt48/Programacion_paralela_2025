#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    const double g = 9.81;  
    int numObjetos = 8;     

    
    vector<double> alturas = {5, 10, 15, 20, 30, 50, 100, 250};
    vector<double> tiempos(numObjetos, 0.0);

    
    #pragma omp parallel for
    for (int i = 0; i < numObjetos; i++) {
        tiempos[i] = sqrt((2 * alturas[i]) / g);
    }

    
    for (int i = 0; i < numObjetos; i++) {
        cout << "Objeto " << i + 1
             << " | Altura: " << alturas[i] << " m"
             << " | Tiempo de caída: " << tiempos[i] << " s" << endl;
    }

    return 0;
}
