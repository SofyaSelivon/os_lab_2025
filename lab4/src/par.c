#include "par.h"
#include <stdlib.h>

void GenerateArray(int *array, int array_size, int seed) {
    srand(seed);
    for (int i = 0; i < array_size; i++) {
        array[i] = rand() % 100;
    }
}

int Sum(const struct SumArgs *args) {
    int sum = 0;
    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];
    }
    return sum;
}
