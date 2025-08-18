#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main() {
    vector<int> datos;
    int x;
    while (cin >> x) datos.push_back(x);

    int n = datos.size();
    int chunk = n/3;
    int fd[3][2];
    for (int i = 0; i < 3; i++) pipe(fd[i]);

    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            close(fd[i][0]);
            int suma = 0;
            for (int j = i*chunk; j < (i+1)*chunk; j++) suma += datos[j];
            write(fd[i][1], &suma, sizeof(suma));
            close(fd[i][1]);
            return 0;
        }
    }

    int total = 0, parcial;
    for (int i = 0; i < 3; i++) {
        close(fd[i][1]);
        read(fd[i][0], &parcial, sizeof(parcial));
        total += parcial;
        close(fd[i][0]);
    }

    while (wait(NULL) > 0);
    cout << "La suma total es: " << total << endl;
    return 0;
}

