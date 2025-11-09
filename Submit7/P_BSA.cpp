#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int parallelBinarySearch(const vector<int>& arr, int x, int P) {
    int n = arr.size();
    int low = 0;
    int high = n - 1;
    bool found = false;
    int position = -1;

    while (low <= high && !found) {
        int segment_size = (high - low + 1) / P;
        if (segment_size == 0) segment_size = 1;

        #pragma omp parallel for num_threads(P) shared(found, position)

        for (int p = 0; p < P; p++) {
            if (found) continue; 
            int start = low + p * segment_size;
            int end = (p == P - 1) ? high : (start + segment_size - 1);

            if (start >= n) continue;


            if (arr[start] == x) {
                found = true;
                position = start;
            } else if (arr[end] == x) {
                found = true;
                position = end;
            }

            else if (arr[start] < x && x < arr[end]) {
                #pragma omp critical
                {
                    low = start;
                    high = end;
                }
            }
        }

        if (segment_size == 1) break; 
    }
    return position;
}

int main() {
    vector<int> arr = {4, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 58, 65, 80, 98};
    int x = 25;  
    int P = 4;   

    cout << "Arreglo ordenado:\n";
    for (int v : arr) cout << v << " ";
    cout << "\n\nBuscando " << x << "\n";

    
    int idx = parallelBinarySearch(arr, x, P);
    

    if (idx != -1)
        cout << "Elemento encontrado en la posicion " << idx << ".\n";
    else
        cout << "Elemento no encontrado.\n";

    return 0;
}
