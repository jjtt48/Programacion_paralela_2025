#include <iostream>
#include <pthread.h>
#include <vector>

const size_t N = 100000000;  
const int NUM_THREADS = 4;   

std::vector<int> arr(N, 1);  
long long sum = 0;           
pthread_mutex_t mutex_sum;

void* partial_sum(void* arg) {
    long thread_id = (long)arg;
    size_t start = (N / NUM_THREADS) * thread_id;
    size_t end   = (thread_id == NUM_THREADS - 1) ? N : (N / NUM_THREADS) * (thread_id + 1);

    long long local_sum = 0;
    for (size_t i = start; i < end; i++) {
        local_sum += arr[i];
    }

    pthread_mutex_lock(&mutex_sum);
    sum += local_sum;
    pthread_mutex_unlock(&mutex_sum);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex_sum, NULL);

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, partial_sum, (void*)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex_sum);

    std::cout << "Suma total (pthread + mutex): " << sum << std::endl;
    return 0;
}
