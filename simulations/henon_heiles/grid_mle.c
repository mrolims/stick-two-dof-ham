#include "../../include/allocs.h"
#include "../../include/chaotic_indicators.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"

#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02

int main(int argc, char *argv[]) {
    double E = get_double(argc, argv, 1);
    double total_time = get_double(argc, argv, 2);
    size_t grid_size = get_size_t(argc, argv, 3);
    size_t n_threads = get_size_t(argc, argv, 4);

    double y_range[] = {-0.5, 1.0};
    double py_range[] = {-0.6, 0.6};

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename),
             "%s/grid_mle_E=%.5f_T=%.1f_grid_size=%zu.dat", path, E, total_time,
             grid_size);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double *mle = xmalloc(grid_size * grid_size, sizeof *mle);
    omp_set_num_threads(n_threads);
#pragma omp parallel for
    for (size_t g = 0; g < grid_size * grid_size; g++) {
        size_t i = g / grid_size;
        size_t j = g % grid_size;

        double y = y_range[0] + i * (y_range[1] - y_range[0]) / (grid_size - 1);
        double py =
            py_range[0] + j * (py_range[1] - py_range[0]) / (grid_size - 1);

        double q[DOF];
        double p[DOF];
        q[0] = 0.0;
        q[1] = y;
        p[1] = py;
        int info = p_from_E(E, q, p, NULL, HH_V);

        if (info == 0) {
            mle[i * grid_size + j] = calculate_mle(
                q, p, NULL, total_time, TIME_STEP, 1, HH_grad_V, HH_hess_V);
        }
    }

    for (size_t i = 0; i < grid_size; i++) {
        double y = y_range[0] + i * (y_range[1] - y_range[0]) / (grid_size - 1);
        for (size_t j = 0; j < grid_size; j++) {
            double py =
                py_range[0] + j * (py_range[1] - py_range[0]) / (grid_size - 1);
            fprintf(fp, "%.16f %.16f %.16e\n", y, py, mle[i * grid_size + j]);
        }
        fprintf(fp, "\n");
    }

    return 0;
}
