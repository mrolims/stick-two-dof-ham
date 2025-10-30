#include "../include/vectors.h"
#include <math.h>

double normalize_vector(double *v, size_t n) {
    double norm = 0.0;
    for (size_t i = 0; i < n; i++)
        norm += v[i] * v[i];
    norm = sqrt(norm);
    for (size_t i = 0; i < n; i++)
        v[i] /= norm;
    return norm;
}

double sum_vector(double *v, size_t n) {
    double sum = 0.0;
    for (size_t i = 0; i < n; i++)
        sum += v[i];
    return sum;
}
