#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    int numVectores = 5;     
    int tam = 1000000;       
    int valorBuscado = 50;   

    
    vector<vector<int>> vectores(numVectores, vector<int>(tam));
    srand(time(NULL));
    for (int i = 0; i < numVectores; i++)
        for (int j = 0; j < tam; j++)
            vectores[i][j] = rand() % 100;

    vector<bool> encontrado(numVectores, false);

    
    #pragma omp parallel for
    for (int i = 0; i < numVectores; i++) {
        for (int j = 0; j < tam; j++) {
            if (vectores[i][j] == valorBuscado) {
                encontrado[i] = true;
                break;  
            }
        }
    }

    
    for (int i = 0; i < numVectores; i++) {
        cout << "Vector " << i + 1 << ": ";
        if (encontrado[i])
            cout << "valor encontrado" << endl;
        else
            cout << "valor no encontrado" << endl;
    }

    return 0;
}
