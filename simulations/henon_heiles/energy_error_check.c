#include "../../include/allocs.h"
#include "../../include/integrator.h"
#include "../../include/models.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TOTAL_TIME 1e4

int main(void) {

    double energies[] = {1.0 / 10.0, 1.0 / 9.0, 1.0 / 8.0, 1.0 / 7.0,
                         1.0 / 6.0};
    double time_step = 0.02;
    size_t num_ic = 500;
    size_t n_steps = (size_t)((double)TOTAL_TIME / time_step);
    double y_range[] = {-0.5, 1.0};
    double py_range[] = {-0.6, 0.6};

    unsigned int seed = 13;
    printf("       energy            rel error\n");
    for (size_t j = 0; j < sizeof(energies) / sizeof(double); j++) {
        double E = energies[j];
        double max_Er = 0.0;
        for (size_t i = 0; i < num_ic; i++) {

            double q[DOF], p[DOF];
            while (1) {
                double rnd = (double)rand_r(&seed) / (double)RAND_MAX;
                double y = y_range[0] + rnd * (y_range[1] - y_range[0]);
                rnd = (double)rand_r(&seed) / (double)RAND_MAX;
                double py = py_range[0] + rnd * (py_range[1] - y_range[0]);

                q[0] = 0.0;
                q[1] = y;
                p[1] = py;
                int info = p_from_E(E, q, p, NULL, HH_V);
                if (info == 0)
                    break;
            }

            double initial_energy =
                (p[0] * p[0] + p[1] * p[1]) / 2.0 + HH_V(q, NULL);
            double *dV = xmalloc(DOF, sizeof *dV);
            for (size_t n = 0; n < n_steps; n++) {
                yoshida4_step(q, p, NULL, time_step, dV, HH_grad_V);
                double current_energy =
                    (p[0] * p[0] + p[1] * p[1]) / 2.0 + HH_V(q, NULL);

                double rel_err =
                    fabs(current_energy - initial_energy) / initial_energy;

                max_Er = fmax(max_Er, rel_err);
            }
            free(dV);
        }

        printf("%.16f %.16e\n", E, max_Er);
    }
}
