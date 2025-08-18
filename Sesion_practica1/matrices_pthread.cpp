#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int N = 40;
vector<vector<int>> A(N, vector<int>(N));
vector<vector<int>> B(N, vector<int>(N));
vector<vector<int>> S(N, vector<int>(N));
vector<vector<int>> R(N, vector<int>(N));
vector<vector<int>> M(N, vector<int>(N));

void* sumar(void* arg) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            S[i][j] = A[i][j] + B[i][j];
    return NULL;
}

void* restar(void* arg) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            R[i][j] = A[i][j] - B[i][j];
    return NULL;
}

int main() {
    srand(time(NULL));

    // Inicializar matrices con 0 o 1
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 2;
            B[i][j] = rand() % 2;
        }

    // Crear hilos
    pthread_t th1, th2;
    pthread_create(&th1, NULL, sumar, NULL);
    pthread_create(&th2, NULL, restar, NULL);

    // Esperar hilos
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    // Multiplicación: M = S * R
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            M[i][j] = 0;
            for (int k = 0; k < N; k++)
                M[i][j] += S[i][k] * R[k][j];
        }

    cout << "Multiplicación completada." << endl;
    return 0;
}
