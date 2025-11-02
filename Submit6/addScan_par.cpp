#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    vector<int> A = {2, 4, 1, 7, 3, 0, 4, 2};
    int N = A.size();
    int num_threads;
    vector<int> sumas;

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nt = omp_get_num_threads();
        int chunk = (N + nt - 1) / nt; 
        int start = id * chunk;
        int end = min(start + chunk, N);

        for (int i = start + 1; i < end; i++)
            A[i] += A[i - 1];

        #pragma omp single
        {
            num_threads = nt;
            sumas.resize(nt, 0);
        }

        sumas[id] = A[end - 1];

        #pragma omp barrier
        int offset = 0;
        for (int i = 0; i < id; i++)
            offset += sumas[i];

        if (id > 0) {
            for (int i = start; i < end; i++)
                A[i] += offset;
        }
    }

    cout << "Prefix sum: ";
    for (int v : A) cout << v << " ";
    cout << endl;
    return 0;
}
