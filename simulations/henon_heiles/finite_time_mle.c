#include "../../include/chaotic_indicators.h"
#include "../../include/models.h"
#include "../../include/parse_args.h"

#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 0.02

int main(int argc, char **argv) {
    double E = get_double(argc, argv, 1);
    double total_time = get_double(argc, argv, 2);
    double finite_time = get_double(argc, argv, 3);
    size_t num_windows = (size_t)(total_time / finite_time);

    const char *home = getenv("HOME");
    char path[128];
    snprintf(path, sizeof(path),
             "%s/Research/stick-two-dof-ham/data/henon_heiles", home);
    char filename[512];
    snprintf(filename, sizeof(filename),
             "%s/finite_time_mle_E=%.5f_T=%.1f_t=%.1f.dat", path, E, total_time,
             finite_time);
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

    for (size_t n = 0; n < num_windows; n++) {
        fprintf(fp, "%.16f %.16f %.16f %.16f ", q[0], q[1], p[0], p[1]);
        double mle = calculate_mle(q, p, NULL, finite_time, TIME_STEP, 1,
                                   HH_grad_V, HH_hess_V);
        fprintf(fp, "%.16e\n", mle);
    }

    fclose(fp);
}
