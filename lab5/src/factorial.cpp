#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <string>

struct ThreadData {
    int start;
    int end;
    unsigned long long partial_result;
    pthread_mutex_t* mutex;
    unsigned long long* global_result;
};

void* partial_factorial(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    unsigned long long result = 1;
    
    for (int i = data->start; i <= data->end; ++i) {
        result *= i;
    }
    
    data->partial_result = result;
    
    pthread_mutex_lock(data->mutex);
    *(data->global_result) *= result;
    pthread_mutex_unlock(data->mutex);
    
    return nullptr;
}

int main(int argc, char* argv[]) {
    int k = 10;
    int pnum = 4;
    int mod = 100;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-k" && i + 1 < argc) k = std::atoi(argv[++i]);
        else if (arg.find("--pnum=") == 0) pnum = std::atoi(arg.substr(7).c_str());
        else if (arg.find("--mod=") == 0) mod = std::atoi(arg.substr(6).c_str());
    }

    if (k < 0 || pnum <= 0 || mod <= 0) {
        std::cerr << "Invalid parameters.\n";
        return 1;
    }

    std::vector<ThreadData> thread_data(pnum);
    std::vector<pthread_t> threads(pnum);
    pthread_mutex_t mutex;
    unsigned long long global_result = 1;

    pthread_mutex_init(&mutex, nullptr);

    int block = k / pnum;
    int remainder = k % pnum;
    int current = 1;

    for (int i = 0; i < pnum; ++i) {
        int start = current;
        int end = start + block - 1;
        if (i < remainder) end++;
        if (end > k) end = k;
        current = end + 1;

        thread_data[i].start = start;
        thread_data[i].end = end;
        thread_data[i].partial_result = 1;
        thread_data[i].mutex = &mutex;
        thread_data[i].global_result = &global_result;

        pthread_create(&threads[i], nullptr, partial_factorial, &thread_data[i]);
    }

    for (int i = 0; i < pnum; ++i) {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&mutex);

    unsigned long long result = global_result % mod;

    std::cout << "factorial " << k << " mod " << mod << " = " << result << "\n";
    return 0;
}