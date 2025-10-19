#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    int numVectores = 5;      
    int tam = 1000000;        

    
    vector<vector<int>> vectores(numVectores, vector<int>(tam));
    srand(time(NULL));
    for (int i = 0; i < numVectores; i++)
        for (int j = 0; j < tam; j++)
            vectores[i][j] = rand() % 100;

    vector<long long> resultados(numVectores, 0);

    
    #pragma omp parallel for
    for (int i = 0; i < numVectores; i++) {
        long long suma = 0;
        for (int j = 0; j < tam; j++)
            suma += vectores[i][j];
        resultados[i] = suma;
    }

    
    for (int i = 0; i < numVectores; i++)
        cout << "Suma del vector " << i + 1 << ": " << resultados[i] << endl;

    return 0;
}
