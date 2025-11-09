#include <iostream>
#include <vector>
using namespace std;

int binarySearch(const vector<int>& arr, int x) {
    int low = 0;
    int high = arr.size() - 1;

    while (low <= high) {
        int mid = (low + high) / 2;

        if (arr[mid] == x)
            return mid;               
        else if (arr[mid] < x)
            low = mid + 1;            
        else
            high = mid - 1;           
    }
    return -1; 
}

int main() {
    vector<int> arr = {4, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 58, 65, 80, 98};
    int x = 25;

    cout << "Arreglo ordenado:\n";
    for (int v : arr) cout << v << " ";
    cout << "\n\nBuscar " << x << "...\n";

    int idx = binarySearch(arr, x);

    if (idx != -1)
        cout << "Elemento encontrado en la posicion " << idx << ".\n";
    else
        cout << "Elemento no encontrado.\n";

    return 0;
}
