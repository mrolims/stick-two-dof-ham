#include "../../include/allocs.h"
#include "../../include/chaotic_indicators.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"

#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02

int main(int argc, char *argv[]) {
    double E = get_double(argc, argv, 1);
    double total_time = get_double(argc, argv, 2);
    size_t num_y = get_size_t(argc, argv, 3);
    double py = get_double(argc, argv, 4);
    size_t n_threads = get_size_t(argc, argv, 5);

    double y_range[] = {-0.5, 1.0};

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename),
             "%s/mle_vs_y_E=%.5f_T=%.1f_num_y=%zu_py=%.6f.dat", path, E,
             total_time, num_y, py);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double *mle = xmalloc(num_y, sizeof *mle);
    omp_set_num_threads(n_threads);
#pragma omp parallel for
    for (size_t i = 0; i < num_y; i++) {

        double y = y_range[0] + i * (y_range[1] - y_range[0]) / (num_y - 1);

        double q[DOF];
        double p[DOF];
        q[0] = 0.0;
        q[1] = y;
        p[1] = py;
        int info = p_from_E(E, q, p, NULL, HH_V);

        if (info == 0) {
            mle[i] = calculate_mle(q, p, NULL, total_time, TIME_STEP, 1,
                                   HH_grad_V, HH_hess_V);
        } else
            mle[i] = NAN;
    }

    for (size_t i = 0; i < num_y; i++) {
        double y = y_range[0] + i * (y_range[1] - y_range[0]) / (num_y - 1);
        fprintf(fp, "%.16f %.16e\n", y, mle[i]);
    }

    return 0;
}
