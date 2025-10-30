#include "../../include/allocs.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"
#include "../../include/pss.h"
#include "../../include/rqa.h"
#include "../../include/vectors.h"

#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02

int main(int argc, char *argv[]) {
    double E = get_double(argc, argv, 1);
    size_t n_cross = get_size_t(argc, argv, 2);
    size_t grid_size = get_size_t(argc, argv, 3);
    size_t n_threads = get_size_t(argc, argv, 4);

    size_t dim_pss = 2 * DOF + 1;
    double y_range[] = {-0.5, 1.0};
    double py_range[] = {-0.6, 0.6};

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename),
             "%s/grid_rte_E=%.5f_N=%zu_grid_size=%zu.dat", path, E, n_cross,
             grid_size);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double *rte = xmalloc(grid_size * grid_size, sizeof *rte);
    omp_set_num_threads(n_threads);
#pragma omp parallel
    {
        double *pss = xmalloc(n_cross * dim_pss, sizeof *pss);
        double *reduced_pss = xmalloc(n_cross * 2, sizeof *pss);
        uint8_t *recmat = xmalloc(n_cross * n_cross, sizeof *recmat);
        double *line_distr = xmalloc(n_cross, sizeof *line_distr);
        double *std = xmalloc(2, sizeof *std);
#pragma omp for
        for (size_t g = 0; g < grid_size * grid_size; g++) {
            size_t i = g / grid_size;
            size_t j = g % grid_size;

            double y =
                y_range[0] + i * (y_range[1] - y_range[0]) / (grid_size - 1);
            double py =
                py_range[0] + j * (py_range[1] - py_range[0]) / (grid_size - 1);

            double q[DOF];
            double p[DOF];
            q[0] = 0.0;
            q[1] = y;
            p[1] = py;
            int info = p_from_E(E, q, p, NULL, HH_V);

            if (info == 0) {
                generate_poincare_section(q, p, NULL, n_cross, TIME_STEP, 0,
                                          0.0, 1, pss, HH_grad_V);
                for (size_t n = 0; n < n_cross; n++) {
                    reduced_pss[n * 2 + 0] = pss[n * dim_pss + 2];
                    reduced_pss[n * 2 + 1] = pss[n * dim_pss + 4];
                }
                std = standard_deviation(n_cross, 2, reduced_pss);
                double eps = 0.1 * fmax(std[0], std[1]);
                rte[i * grid_size + j] =
                    recurrence_time_entropy(n_cross, 2, reduced_pss, eps, 1,
                                            recmat, line_distr, NORM_MAX);
            } else
                rte[i * grid_size + j] = NAN;
        }

        free(pss);
        free(reduced_pss);
        free(recmat);
        free(line_distr);
        free(std);
    }

    for (size_t i = 0; i < grid_size; i++) {
        double y = y_range[0] + i * (y_range[1] - y_range[0]) / (grid_size - 1);
        for (size_t j = 0; j < grid_size; j++) {
            double py =
                py_range[0] + j * (py_range[1] - py_range[0]) / (grid_size - 1);
            fprintf(fp, "%.16f %.16f %.16e\n", y, py, rte[i * grid_size + j]);
        }
        fprintf(fp, "\n");
    }

    return 0;
}
