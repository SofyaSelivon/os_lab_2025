#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_func(void* arg) {
    std::cout << "thread 1: locking mutex1\n";
    pthread_mutex_lock(&mutex1);
    sleep(1);
    std::cout << "thread 1: trying to lock mutex2\n";
    pthread_mutex_lock(&mutex2);
    std::cout << "thread 1: locked mutex2\n";
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    return nullptr;
}

void* thread2_func(void* arg) {
    std::cout << "thread 2: locking mutex2\n";
    pthread_mutex_lock(&mutex2);
    sleep(1);
    std::cout << "thread 2: trying to lock mutex1\n";
    pthread_mutex_lock(&mutex1);
    std::cout << "thread 2: locked mutex1\n";
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    return nullptr;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, nullptr, thread1_func, nullptr);
    pthread_create(&t2, nullptr, thread2_func, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    return 0;
}
