#include <iostream>
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

int main() {
    srand(time(NULL));

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 2;
            B[i][j] = rand() % 2;
        }

    // Calcular suma y resta en el mismo hilo
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            S[i][j] = A[i][j] + B[i][j];
            R[i][j] = A[i][j] - B[i][j];
        }

    // Multiplicación: M = S * R
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            M[i][j] = 0;
            for (int k = 0; k < N; k++)
                M[i][j] += S[i][k] * R[k][j];
        }

    cout << "Multiplicación completada (versión secuencial)." << endl;
    return 0;
}
