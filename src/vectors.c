#include "../include/vectors.h"

#include "../include/allocs.h"

#include <math.h>
#include <stdlib.h>

double *standard_deviation(size_t N, size_t dim, const double *x) {

    double *mean = xmalloc(dim, sizeof *mean);
    double *std = xmalloc(dim, sizeof *std);

    // Initialize sums
    for (size_t d = 0; d < dim; d++) {
        mean[d] = 0.0;
        std[d] = 0.0;
    }

    // Compute means
    for (size_t i = 0; i < N; i++) {
        for (size_t d = 0; d < dim; d++) {
            mean[d] += x[i * dim + d];
        }
    }
    for (size_t d = 0; d < dim; d++)
        mean[d] /= N;

    // Compute variance
    for (size_t i = 0; i < N; i++) {
        for (size_t d = 0; d < dim; d++) {
            double diff = x[i * dim + d] - mean[d];
            std[d] += diff * diff;
        }
    }
    for (size_t d = 0; d < dim; d++)
        std[d] = sqrt(std[d] / N);

    free(mean);
    return std;
}

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
