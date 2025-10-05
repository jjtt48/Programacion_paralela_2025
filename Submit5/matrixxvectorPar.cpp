#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

int main(int argc, char* argv[]) {
    int m = 4, n = 4;
    if (argc == 3) {
        m = atoi(argv[1]);
        n = atoi(argv[2]);
    }
    srand(time(nullptr));
    vector<vector<int>> A(m, vector<int>(n));
    vector<int> x(n), result(m, 0);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            A[i][j] = rand() % 10;

    for (int j = 0; j < n; j++)
        x[j] = rand() % 10;
    
    cout << "Matriz A (" << m << "x" << n << "):\n";
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            cout << A[i][j] << " ";
        cout << endl;
    }

    cout << "\nVector x:\n";
    for (int j = 0; j < n; j++)
        cout << x[j] << " ";
    cout << endl;
    #pragma omp parallel for
    for (int i = 0; i < m; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++)
            sum += A[i][j] * x[j];
        result[i] = sum;
    }

    cout << "Resultado Ax (paralelo):\n";
    for (int i = 0; i < m; i++)
        cout << result[i] << " ";
    cout << endl;

    return 0;
}
