#include "../../include/allocs.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"
#include "../../include/pss.h"
#include "../../include/rqa.h"
#include "../../include/vectors.h"

#include <inttypes.h>
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02
#define THRESHOLD 0.05

int main(int argc, char **argv) {

    size_t ic_num = get_size_t(argc, argv, 1);

    double E = 1.0 / 9.0;
    size_t n_cross = 5000;

    size_t dim_pss = 2 * DOF + 1;

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename), "%s/recmat_E=%.5f_N=%zu_ic=%zu.dat",
             path, E, n_cross, ic_num);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    snprintf(filename, sizeof(filename), "%s/pss_E=%.5f_N=%zu_ic=%zu.dat", path,
             E, n_cross, ic_num);
    FILE *fp2 = fopen(filename, "w");
    if (!fp2) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double *pss = xmalloc(n_cross * dim_pss, sizeof *pss);
    double *reduced_pss = xmalloc(n_cross * 2, sizeof *pss);
    uint8_t *recmat = xmalloc(n_cross * n_cross, sizeof *recmat);
    double *line_distr = xmalloc(n_cross, sizeof *line_distr);
    double *std = xmalloc(2, sizeof *std);

    double y, py;
    switch (ic_num) {
    case (1):
        y = 0.0;
        py = 0.0;
        break;
    case (2):
        y = -0.15;
        py = 0.0;
        break;
    case (3):
        y = -0.055;
        py = 0.0;
        break;
    default:
        fprintf(stderr, "Invalid IC num\n");
        exit(EXIT_FAILURE);
    }

    double q[DOF];
    double p[DOF];
    q[0] = 0.0;
    q[1] = y;
    p[1] = py;
    int info = p_from_E(E, q, p, NULL, HH_V);
    if (info != 0) {
        fprintf(stderr, "Error: invalid IC!");
        exit(EXIT_FAILURE);
    }

    generate_poincare_section(q, p, NULL, n_cross, TIME_STEP, 0, 0.0, 1, pss,
                              HH_grad_V);
    for (size_t n = 0; n < n_cross; n++) {
        reduced_pss[n * 2 + 0] = pss[n * dim_pss + 2];
        reduced_pss[n * 2 + 1] = pss[n * dim_pss + 4];
    }

    for (size_t i = 0; i < n_cross; i++) {
        fprintf(fp2, "%.16f %.16f\n", reduced_pss[i * 2 + 0],
                reduced_pss[i * 2 + 1]);
    }
    fclose(fp2);

    std = standard_deviation(n_cross, 2, reduced_pss);
    double eps = THRESHOLD * fmax(std[0], std[1]);

    recurrence_matrix(n_cross, 2, reduced_pss, eps, recmat, NORM_MAX);

    double rte = recurrence_time_entropy(n_cross, 2, reduced_pss, eps, 1,
                                         recmat, line_distr, NORM_MAX);

    printf("RTE = %.16f\n", rte);

    for (size_t i = 0; i < n_cross; i++) {
        for (size_t j = 0; j < n_cross; j++) {
            fprintf(fp, "%zu %zu %" PRIu8 "\n", i, j, recmat[i * n_cross + j]);
        }
    }

    fclose(fp);
    free(pss);
    free(reduced_pss);
    free(recmat);
    free(line_distr);
    free(std);

    return 0;
}
