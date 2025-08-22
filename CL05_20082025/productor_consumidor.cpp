#include <iostream>
#include <omp.h>
#include <queue>
using namespace std;

queue<int> buffer;
const int N = 10;

int main() {
    #pragma omp parallel sections
    {
        #pragma omp section   // Productor
        {
            for (int i = 0; i < 20; i++) {
                while (buffer.size() == N); 
                #pragma omp critical
                buffer.push(i);
            }
        }
        #pragma omp section   // Consumidor
        {
            for (int i = 0; i < 20; i++) {
                while (buffer.empty()); 
                int item;
                #pragma omp critical
                {
                    item = buffer.front();
                    buffer.pop();
                }
                cout << "Consumido: " << item << endl;
            }
        }
    }
}
