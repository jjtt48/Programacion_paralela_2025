#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>
using namespace std;

void printMatrix(const vector<vector<int>>& M) {
    for (auto& row : M) {
        for (int val : row) cout << val << "\t";
        cout << "\n";
    }
    cout << "\n";
}

void sort_row(vector<vector<int>>& M, int r, bool ascending) {
    if (ascending)
        sort(M[r].begin(), M[r].end());
    else
        sort(M[r].rbegin(), M[r].rend());
}

void sort_column(vector<vector<int>>& M, int c) {
    int n = M.size();
    vector<int> col(n);
    for (int i = 0; i < n; i++) col[i] = M[i][c];
    sort(col.begin(), col.end());
    for (int i = 0; i < n; i++) M[i][c] = col[i];
}

void shearSort_parallel(vector<vector<int>>& M) {
    int n = M.size();
    int rounds = floor(log2(n)) + 1;

    for (int r = 0; r < rounds; r++) {
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            bool asc = (i % 2 == 0);
            sort_row(M, i, asc);
        }

        #pragma omp parallel for
        for (int j = 0; j < n; j++) {
            sort_column(M, j);
        }
    }
}

int main() {
    vector<vector<int>> M = {
        {12, 7, 9, 3},
        {5, 15, 8, 2},
        {11, 4, 1, 10},
        {6, 13, 14, 0}
    };

    cout << "Matriz original:\n";
    printMatrix(M);
    
    shearSort_parallel(M);

    cout << "Matriz ordenada:\n";
    printMatrix(M);
    

    return 0;
}
