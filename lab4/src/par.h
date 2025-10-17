#ifndef PAR_H
#define PAR_H

struct SumArgs {
    int *array;
    int begin;
    int end;
};

void GenerateArray(int *array, int array_size, int seed);
int Sum(const struct SumArgs *args);

#endif
