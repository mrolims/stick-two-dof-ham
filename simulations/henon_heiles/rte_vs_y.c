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
    size_t num_y = get_size_t(argc, argv, 3);
    double py = get_double(argc, argv, 4);
    double threshold = get_double(argc, argv, 5);
    size_t n_threads = get_size_t(argc, argv, 6);

    size_t dim_pss = 2 * DOF + 1;
    double y_range[] = {-0.5, 1.0};

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename),
             "%s/rte_vs_y_E=%.5f_N=%zu_num_y=%zu_py=%.5f_eps=%.3f.dat", path, E,
             n_cross, num_y, py, threshold);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double *rte = xmalloc(num_y, sizeof *rte);
    omp_set_num_threads(n_threads);
#pragma omp parallel
    {
        double *pss = xmalloc(n_cross * dim_pss, sizeof *pss);
        double *reduced_pss = xmalloc(n_cross * 2, sizeof *pss);
        uint8_t *recmat = xmalloc(n_cross * n_cross, sizeof *recmat);
        double *line_distr = xmalloc(n_cross, sizeof *line_distr);
        double *std = xmalloc(2, sizeof *std);
#pragma omp for
        for (size_t i = 0; i < num_y; i++) {

            double y = y_range[0] + i * (y_range[1] - y_range[0]) / (num_y - 1);

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
                double eps = threshold * fmax(std[0], std[1]);
                rte[i] =
                    recurrence_time_entropy(n_cross, 2, reduced_pss, eps, 1,
                                            recmat, line_distr, NORM_MAX);
            } else
                rte[i] = NAN;
        }

        free(pss);
        free(reduced_pss);
        free(recmat);
        free(line_distr);
        free(std);
    }

    for (size_t i = 0; i < num_y; i++) {
        double y = y_range[0] + i * (y_range[1] - y_range[0]) / (num_y - 1);
        fprintf(fp, "%.16f %.16e\n", y, rte[i]);
    }

    return 0;
}
