#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

void printMatrix(const vector<vector<int>>& M) {
    for (auto& row : M) {
        for (int val : row)
            cout << val << "\t";
        cout << "\n";
    }
    cout << "\n";
}

void sort_row(vector<vector<int>>& M) {
    int n = M.size();
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0)
            sort(M[i].begin(), M[i].end());       
        else
            sort(M[i].rbegin(), M[i].rend());     
    }
}


void sort_column(vector<vector<int>>& M) {
    int n = M.size();
    for (int j = 0; j < n; j++) {
        vector<int> col(n);
        for (int i = 0; i < n; i++) col[i] = M[i][j];
        sort(col.begin(), col.end());
        for (int i = 0; i < n; i++) M[i][j] = col[i];
    }
}

void transpose(vector<vector<int>>& M) {
    int n = M.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            swap(M[i][j], M[j][i]);
        }
    }
}

void shearSort_modular(vector<vector<int>>& M) {
    int n = M.size();
    int rounds = floor(log2(n)) + 1;

    for (int r = 0; r < rounds; r++) {
        sort_row(M);     
        sort_column(M);  
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

    shearSort_modular(M);

    cout << "Matriz ordenada (ShearSort Modular):\n";
    printMatrix(M);

    return 0;
}
