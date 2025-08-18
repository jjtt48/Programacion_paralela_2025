#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main() {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == 0) {
        // Hijo
        close(fd[0]);
        int suma = 0;
        for (int i = 1; i <= 10; i++) suma += i;
        write(fd[1], &suma, sizeof(suma));
        close(fd[1]);
    } else {
        // Padre
        close(fd[1]);
        int resultado;
        read(fd[0], &resultado, sizeof(resultado));
        close(fd[0]);
        wait(NULL);
        cout << "La suma es: " << resultado << endl;
    }
    return 0;
}
