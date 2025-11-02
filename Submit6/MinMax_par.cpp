#include <iostream>
#include <omp.h>
#include <vector>
using namespace std;

int main() {
    vector<int> A = {3, 4, 5, 6, 3, 1, 2, 4};
    int N = A.size();

    int global_max = A[0];
    int global_min = A[0];

    #pragma omp parallel
    {
        int local_max = A[0];
        int local_min = A[0];

        #pragma omp for nowait
        for (int i = 0; i < N; i++) {
            if (A[i] > local_max) local_max = A[i];
            if (A[i] < local_min) local_min = A[i];
        }

        #pragma omp critical
        {
            if (local_max > global_max) global_max = local_max;
            if (local_min < global_min) global_min = local_min;
        }
    }
    cout << "Maximum = " << global_max << endl;
    cout << "Minimum = " << global_min << endl;
    return 0;
}
