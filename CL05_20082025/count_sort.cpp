#include <iostream>
#include <omp.h>
using namespace std;

#define N 10

int main() {
    int A[N], B[N];

    // Inicializar arreglo
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 100;
        cout << A[i] << " ";
    }
    cout << "\n";

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        int count = 0;
        for (int j = 0; j < N; j++) {
            if (A[j] < A[i] || (A[j] == A[i] && j < i))
                count++;
        }
        B[count] = A[i];
    }

    cout << "Ordenado:\n";
    for (int i = 0; i < N; i++)
        cout << B[i] << " ";
    cout << endl;
}
