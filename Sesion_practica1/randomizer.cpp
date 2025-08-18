#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    srand(time(NULL));
    int n = 9;
    for (int i = 0; i < n; i++) {
        cout << rand() % 100 << " ";
    }
    cout << endl;
    return 0;
}
