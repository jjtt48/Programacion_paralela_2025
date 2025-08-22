#include <iostream>
#include <omp.h>
using namespace std;

#define N 6
#define M 5

int main() {
    double A[N][M], x[M], y[N];

    // Inicializar
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            A[i][j] = rand() % 10;
    for (int j = 0; j < M; j++)
        x[j] = rand() % 10;

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        y[i] = 0;
        for (int j = 0; j < M; j++)
            y[i] += A[i][j] * x[j];
    }

    cout << "Resultado y = A*x:\n";
    for (int i = 0; i < N; i++)
        cout << y[i] << " ";
    cout << endl;
}
