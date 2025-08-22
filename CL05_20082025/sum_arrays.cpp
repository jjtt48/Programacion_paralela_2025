#include <omp.h>
#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>  
#include <ctime>     


int main(){
    const int N = 100000000;
    std::vector<int> arr(N,1); 
    long long sum = 0;
    # pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++){
        sum+= arr[i];
    }
    
    std::cout << "Suma total: " << sum << std::endl;

}