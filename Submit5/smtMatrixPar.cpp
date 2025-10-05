#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;
int N = 4; 
vector<vector<double>> A, B, C_add, C_mul, T_A;
pthread_barrier_t barrier;

void init_matrices() {
    A.assign(N, vector<double>(N));
    B.assign(N, vector<double>(N));
    C_add.assign(N, vector<double>(N, 0.0));
    C_mul.assign(N, vector<double>(N, 0.0));
    T_A.assign(N, vector<double>(N, 0.0));
    srand((unsigned) time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
}

void print_matrix(const vector<vector<double>> &M, const string &name) {
    cout << "\n" << name << ":\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            cout << setw(6) << M[i][j] << " ";
        cout << "\n";
    }
}

void* task_add(void* arg) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C_add[i][j] = A[i][j] + B[i][j];
    cout << "[Thread 1] Addition done.\n";
    pthread_barrier_wait(&barrier);
    return NULL;
}

void* task_mul(void* arg) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C_mul[i][j] = 0;
            for (int k = 0; k < N; k++)
                C_mul[i][j] += A[i][k] * B[k][j];
        }
    cout << "[Thread 2] Multiplication done.\n";
    pthread_barrier_wait(&barrier);
    return NULL;
}

void* task_transpose(void* arg) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            T_A[j][i] = A[i][j];
    cout << "[Thread 3] Transpose done.\n";
    pthread_barrier_wait(&barrier);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        N = atoi(argv[1]);
        if (N <= 0) {
            cerr << "Matrix size must be positive.\n";
            return 1;
        }
    }
    cout << "Matrix size: " << N << "x" << N << endl;
    init_matrices();
    pthread_t threads[3];
    pthread_barrier_init(&barrier, NULL, 3);
    pthread_create(&threads[0], NULL, task_add, NULL);
    pthread_create(&threads[1], NULL, task_mul, NULL);
    pthread_create(&threads[2], NULL, task_transpose, NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(threads[i], NULL);

    pthread_barrier_destroy(&barrier);
    print_matrix(A, "Matrix A");
    print_matrix(B, "Matrix B");
    print_matrix(C_add, "A + B");
    print_matrix(C_mul, "A * B");
    print_matrix(T_A, "Transpose(A)");
    return 0;
}
