#include "../../include/allocs.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"
#include "../../include/pss.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02

int main(int argc, char *argv[]) {

    double E = get_double(argc, argv, 1);
    size_t n_cross = get_size_t(argc, argv, 2);
    size_t num_ic = get_size_t(argc, argv, 3);

    size_t dim_pss = 2 * DOF + 1;
    double y_range[] = {-0.5, 1.0};
    double py_range[] = {-0.6, 0.6};

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename), "%s/pss_E=%.5f_N=%zu_num_ic=%zu.dat",
             path, E, n_cross, num_ic);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    unsigned int seed = 13;
    for (size_t i = 0; i < num_ic; i++) {

        double q[DOF], p[DOF];
        while (1) {
            double rnd = (double)rand_r(&seed) / (double)RAND_MAX;
            double y = y_range[0] + rnd * (y_range[1] - y_range[0]);
            rnd = (double)rand_r(&seed) / (double)RAND_MAX;
            double py = py_range[0] + rnd * (py_range[1] - py_range[0]);

            q[0] = 0.0;
            q[1] = y;
            p[1] = py;
            int info = p_from_E(E, q, p, NULL, HH_V);

            if (info == 0)
                break;
        }

        double *pss = xmalloc(n_cross * dim_pss, sizeof *pss);
        generate_poincare_section(q, p, NULL, n_cross, TIME_STEP, 0, 0.0, 1,
                                  pss, HH_grad_V);

        for (size_t j = 0; j < n_cross; j++) {
            for (size_t k = 0; k < dim_pss; k++) {
                fprintf(fp, "%.16f ", pss[j * dim_pss + k]);
            }
            fprintf(fp, "\n");
        }
        free(pss);
    }

    return 0;
}
