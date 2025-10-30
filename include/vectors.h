#ifndef VECTORS_H
#define VECTORS_H

#include <stddef.h>

double *standard_deviation(size_t N, size_t dim, const double *x);

double normalize_vector(double *v, size_t n);

double sum_vector(double *v, size_t n);

#endif
