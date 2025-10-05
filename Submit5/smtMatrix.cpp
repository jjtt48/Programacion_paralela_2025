#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <matrix_size>\n";
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        cerr << "Matrix size must be positive.\n";
        return 1;
    }

    vector<vector<double>> A(N, vector<double>(N));
    vector<vector<double>> B(N, vector<double>(N));
    vector<vector<double>> C_add(N, vector<double>(N, 0.0));
    vector<vector<double>> C_mul(N, vector<double>(N, 0.0));
    vector<vector<double>> T_A(N, vector<double>(N, 0.0));

    srand((unsigned) time(NULL));

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C_add[i][j] = A[i][j] + B[i][j];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C_mul[i][j] = 0.0;
            for (int k = 0; k < N; k++)
                C_mul[i][j] += A[i][k] * B[k][j];
        }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            T_A[j][i] = A[i][j];

    auto printMatrix = [&](const vector<vector<double>> &M, const string &name) {
        cout << "\n" << name << ":\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                cout << setw(6) << M[i][j] << " ";
            cout << "\n";
        }
    };

    printMatrix(A, "Matrix A");
    printMatrix(B, "Matrix B");
    printMatrix(C_add, "A + B");
    printMatrix(C_mul, "A * B");
    printMatrix(T_A, "Transpose(A)");

    return 0;
}
