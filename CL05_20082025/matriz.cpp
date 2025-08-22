#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

using namespace std;

#define MAX 100        // Tamaño máximo de las matrices
#define NUM_THREADS 4  // Número de hilos

// Estructura para pasar datos a cada hilo
struct ThreadData {
    int start_row;
    int end_row;
    int n, m, p;
    double (*A)[MAX];
    double (*B)[MAX];
    double (*C)[MAX];
};

// Generador de matrices con valores aleatorios
void generate_matrix(int rows, int cols, double matrix[MAX][MAX]) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            matrix[i][j] = rand() % 10; // valores entre 0-9
}

// Función que ejecutará cada hilo
void* multiply(void* arg) {
    ThreadData* data = (ThreadData*) arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->p; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < data->m; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    int n = 6, m = 5, p = 4;  // dimensiones: A(nxm), B(mxp), C(nxp)
    double A[MAX][MAX], B[MAX][MAX], C[MAX][MAX];

    // Generar matrices
    generate_matrix(n, m, A);
    generate_matrix(m, p, B);

    cout << "Matriz A:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) cout << A[i][j] << " ";
        cout << endl;
    }

    cout << "\nMatriz B:" << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) cout << B[i][j] << " ";
        cout << endl;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int rows_per_thread = n / NUM_THREADS;
    int extra = n % NUM_THREADS;
    int start = 0;

    // Crear hilos
    for (int t = 0; t < NUM_THREADS; t++) {
        int rows = rows_per_thread + (t < extra ? 1 : 0);

        thread_data[t].start_row = start;
        thread_data[t].end_row = start + rows;
        thread_data[t].n = n;
        thread_data[t].m = m;
        thread_data[t].p = p;
        thread_data[t].A = A;
        thread_data[t].B = B;
        thread_data[t].C = C;

        pthread_create(&threads[t], NULL, multiply, (void*)&thread_data[t]);
        start += rows;
    }

    // Esperar a que los hilos terminen
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // Imprimir resultado
    cout << "\nMatriz C = A x B:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) cout << C[i][j] << " ";
        cout << endl;
    }

    return 0;
}

