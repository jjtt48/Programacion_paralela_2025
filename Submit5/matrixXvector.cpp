#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

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

    
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            result[i] += A[i][j] * x[j];

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

    cout << "\nResultado Ax:\n";
    for (int i = 0; i < m; i++)
        cout << result[i] << " ";
    cout << endl;

    return 0;
}
