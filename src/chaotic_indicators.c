#include "../include/allocs.h"
#include "../include/integrator.h"
#include "../include/linalg.h"
#include "../include/models.h"
#include "../include/types.h"
#include "../include/vectors.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lyapunov_spectrum(double *q, double *p, const double *parameters,
                       size_t num_exponents, double total_time,
                       double time_step, size_t qr_interval, double *lyap_out,
                       gradV_func_t dVdq, hessV_func_t hessianV) {

    size_t dim = 2 * DOF;

    if (num_exponents > dim) {
        fprintf(stderr, "num_exponents must be less or equal dim");
        exit(1);
    }

    size_t n_steps = (size_t)(total_time / time_step);

    // Deviation vector matrix: Column-major (dim rows, num_exponents columns)
    double *X = xmalloc(dim * num_exponents, sizeof *X);
    // R matrix: Column-major (num_exponents rows, num_exponents columns)
    double *R = xmalloc(num_exponents * num_exponents, sizeof *R);

    // Initialize the deviation matrix randomly
    for (size_t i = 0; i < dim * num_exponents; i++)
        X[i] = (double)rand() / (double)RAND_MAX;

    // Orthonormalize the deviation vectors
    qr_colmajor(dim, num_exponents, X, R);

    for (size_t i = 0; i < num_exponents; i++) {
        lyap_out[i] = 0.0;
    }

    double *dV = xmalloc(DOF, sizeof *dV);
    double *Hv = xmalloc(DOF * DOF, sizeof *Hv);
    for (size_t n = 0; n < n_steps; n++) {

        yoshida4_step_traj_tan(q, p, X, parameters, time_step, num_exponents,
                               dV, Hv, dVdq, hessianV);

        if ((n + 1) % qr_interval == 0) {
            qr_colmajor(dim, num_exponents, X, R);

            for (size_t i = 0; i < num_exponents; i++) {
                double rii = R[i + i * num_exponents];
                if (rii != 0.0)
                    lyap_out[i] += log(fabs(rii));
            }
        }
    }

    for (size_t i = 0; i < num_exponents; i++)
        lyap_out[i] /= total_time;

    free(X);
    free(R);
    free(dV);
    free(Hv);
}

double calculate_mle(double *q, double *p, const double *parameters,
                     double total_time, double time_step, size_t rescale_every,
                     gradV_func_t dVdq, hessV_func_t hessianV) {

    size_t n_steps = (size_t)(total_time / time_step);
    size_t dim = 2 * DOF;
    double *X = xmalloc(dim, sizeof *X);
    double *dV = xmalloc(DOF, sizeof *dV);
    double *Hv = xmalloc(DOF * DOF, sizeof *Hv);

    for (size_t i = 0; i < dim; i++) {
        X[i] = (double)rand() / (double)RAND_MAX;
    }
    normalize_vector(X, dim);
    double sum_log = 0.0;

    for (size_t n = 0; n < n_steps; n++) {
        yoshida4_step_traj_tan(q, p, X, parameters, time_step, 1, dV, Hv, dVdq,
                               hessianV);

        if ((n + 1) % rescale_every == 0) {
            double norm = normalize_vector(X, dim);
            sum_log += log(norm);
        }
    }

    free(X);
    free(dV);
    free(Hv);

    return sum_log / total_time;
}
