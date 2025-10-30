
#include "../include/rqa.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void recurrence_matrix(size_t N, size_t dim, const double *x, double eps,
                       uint8_t *R, NormType norm_type) {

    for (size_t i = 0; i < N; i++) {
        for (size_t j = i; j < N; j++) {

            double dist = 0.0;

            switch (norm_type) {
            case NORM_MAX: {
                for (size_t d = 0; d < dim; d++) {
                    double diff = fabs(x[i * dim + d] - x[j * dim + d]);
                    if (diff > dist)
                        dist = diff;
                }
                break;
            }

            case NORM_L1: {
                for (size_t d = 0; d < dim; d++)
                    dist += fabs(x[i * dim + d] - x[j * dim + d]);
                break;
            }

            case NORM_L2: {
                for (size_t d = 0; d < dim; d++) {
                    double diff = x[i * dim + d] - x[j * dim + d];
                    dist += diff * diff;
                }
                dist = sqrt(dist);
                break;
            }
            }

            uint8_t value = (dist <= eps);
            R[i * N + j] = value;
            R[j * N + i] = value;
        }
    }
}

void distr_white_vert_lines(size_t N, const uint8_t *recmat,
                            size_t min_line_length, double *line_dist) {
    memset(line_dist, 0, N * sizeof(double));

    for (size_t col = 0; col < N; col++) {
        size_t current_white_length = 0;
        size_t first_black_seen = 0;

        for (size_t row = 0; row < N; row++) {
            size_t cell = recmat[row * N + col];

            if (cell == 0 && first_black_seen) {
                current_white_length++;
            } else {
                if (current_white_length >= min_line_length) {
                    line_dist[current_white_length - 1] += 1.0;
                }
                current_white_length = 0;

                if (cell == 1) {
                    first_black_seen = 1;
                }
            }
        }
    }
}

double recurrence_time_entropy(size_t N, size_t dim, const double *x,
                               double eps, size_t min_line_length,
                               uint8_t *recmat, double *line_distr,
                               NormType norm_type) {

    recurrence_matrix(N, dim, x, eps, recmat, norm_type);
    distr_white_vert_lines(N, recmat, min_line_length, line_distr);

    double sum_counts = 0.0;
    for (size_t i = 0; i < N; i++) {
        sum_counts += line_distr[i];
    }

    if (sum_counts == 0)
        return 0.0;

    double entropy = 0;
    for (size_t i = 0; i < N; i++) {
        if (line_distr[i] > 0) {
            double p = line_distr[i] / sum_counts;
            entropy += p * log(p);
        }
    }

    return -entropy;
}
