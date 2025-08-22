#include <iostream>
#include <omp.h>
#include <cmath>
using namespace std;

double f(double x) { return x*x; }

int main() {
    int n = 1024;
    double a = 0.0, b = 1.0;
    double h = (b - a) / n;
    double integral = (f(a) + f(b)) / 2.0;

    #pragma omp parallel for reduction(+:integral)
    for (int i = 1; i < n; i++) {
        double x = a + i*h;
        integral += f(x);
    }

    integral *= h;
    cout << "Integral ≈ " << integral << endl;
}
