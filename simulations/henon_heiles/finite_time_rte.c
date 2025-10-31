#include "../../include/allocs.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"
#include "../../include/pss.h"
#include "../../include/rqa.h"
#include "../../include/vectors.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02

int main(int argc, char **argv) {
    double E = get_double(argc, argv, 1);
    size_t total_crossings = get_size_t(argc, argv, 2);
    size_t finite_crossings = get_size_t(argc, argv, 3);
    double threshold = get_double(argc, argv, 4);
    size_t num_windows = (size_t)(total_crossings / finite_crossings);

    size_t dim_pss = 2 * DOF + 1;
    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename),
             "%s/finite_time_rte_E=%.5f_T=%zu_t=%zu_threshold=%.3f.dat", path,
             E, total_crossings, finite_crossings, threshold);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("%s\n", filename);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double q[DOF], p[DOF];
    q[0] = 0.0;
    q[1] = -0.15;
    p[1] = 0.0;
    int info = p_from_E(E, q, p, NULL, HH_V);

    if (info != 0) {
        fprintf(stderr, "Error: invalid IC\n");
        exit(EXIT_FAILURE);
    }

    double *pss = xmalloc(finite_crossings * dim_pss, sizeof *pss);
    double *reduced_pss = xmalloc(finite_crossings * 2, sizeof *reduced_pss);
    uint8_t *recmat =
        xmalloc(finite_crossings * finite_crossings, sizeof *recmat);
    double *line_distr = xmalloc(finite_crossings, sizeof *line_distr);
    double *std = xmalloc(2, sizeof *std);

    for (size_t n = 0; n < num_windows; n++) {
        fprintf(fp, "%.16f %.16f %.16f %.16f ", q[0], q[1], p[0], p[1]);
        generate_poincare_section(q, p, NULL, finite_crossings, TIME_STEP, 0,
                                  0.0, 1, pss, HH_grad_V);
        for (size_t m = 0; m < finite_crossings; m++) {
            reduced_pss[m * 2 + 0] = pss[m * dim_pss + 2];
            reduced_pss[m * 2 + 1] = pss[m * dim_pss + 4];
        }
        std = standard_deviation(finite_crossings, 2, reduced_pss);
        double eps = threshold * fmax(std[0], std[1]);
        double rte =
            recurrence_time_entropy(finite_crossings, 2, reduced_pss, eps, 1,
                                    recmat, line_distr, NORM_MAX);
        fprintf(fp, "%.16f\n", rte);
    }

    fclose(fp);
}
