#include "../include/models.h"
#include "../include/types.h"
#include <stddef.h>
#include <stdlib.h>

static const double ALPHA =
    1.0 / (2.0 - 1.25992104989487); // ~0.6756035959798289
static const double BETA =
    -1.25992104989487 / (2.0 - 1.25992104989487); // ~-0.3512071919596578

void symplectic_step(double *q, double *p, const double *parameters,
                     const double time_step, double *dV, gradV_func_t dVdq) {

    // Half kick
    dVdq(dV, q, parameters);
    for (size_t i = 0; i < DOF; i++)
        p[i] -= 0.5 * time_step * dV[i];

    // Drift
    for (size_t i = 0; i < DOF; i++)
        q[i] += time_step * p[i];

    // Half kick
    dVdq(dV, q, parameters);
    for (size_t i = 0; i < DOF; i++)
        p[i] -= 0.5 * time_step * dV[i];
}

void symplectic_step_traj_tan(double *q, double *p, double *X,
                              const double *parameters, double time_step,
                              size_t n_dev, double *dV, double *hess_V,
                              gradV_func_t dVdq, hessV_func_t hessian_V) {

    // --- Half kick on main trajectory ---
    dVdq(dV, q, parameters); // gradient at current q
    for (size_t i = 0; i < DOF; i++)
        p[i] -= 0.5 * time_step * dV[i];

    // --- Half kick on tangent momenta ---
    hessian_V(hess_V, q, parameters); // Hessian at current q
    for (size_t k = 0; k < n_dev; k++) {
        double *Xk = X + k * 2 * DOF;
        double *dq = Xk;
        double *dp = Xk + DOF;

        for (size_t i = 0; i < DOF; i++) {
            double tmp = 0.0;
            for (size_t j = 0; j < DOF; j++)
                tmp += hess_V[i + j * DOF] * dq[j]; // column-major
            dp[i] -= 0.5 * time_step * tmp;
        }
    }

    // --- Drift ---
    for (size_t i = 0; i < DOF; i++)
        q[i] += time_step * p[i];

    // --- Drift on tangent positions ---
    for (size_t k = 0; k < n_dev; k++) {
        double *Xk = X + k * 2 * DOF;
        double *dq = Xk;
        double *dp = Xk + DOF;
        for (size_t i = 0; i < DOF; i++)
            dq[i] += time_step * dp[i];
    }

    // --- Half kick on main trajectory ---
    dVdq(dV, q, parameters);
    for (size_t i = 0; i < DOF; i++)
        p[i] -= 0.5 * time_step * dV[i];

    // --- Half kick on tangent momenta ---
    hessian_V(hess_V, q, parameters);
    for (size_t k = 0; k < n_dev; k++) {
        double *Xk = X + k * 2 * DOF;
        double *dq = Xk;
        double *dp = Xk + DOF;

        for (size_t i = 0; i < DOF; i++) {
            double tmp = 0.0;
            for (size_t j = 0; j < DOF; j++)
                tmp += hess_V[i + j * DOF] * dq[j];
            dp[i] -= 0.5 * time_step * tmp;
        }
    }
}

void yoshida4_step(double *q, double *p, const double *parameters,
                   const double time_step, double *dV, gradV_func_t dVdq) {
    symplectic_step(q, p, parameters, ALPHA * time_step, dV, dVdq);
    symplectic_step(q, p, parameters, BETA * time_step, dV, dVdq);
    symplectic_step(q, p, parameters, ALPHA * time_step, dV, dVdq);
}

void yoshida4_step_traj_tan(double *q, double *p, double *X,
                            const double *parameters, double time_step,
                            size_t n_dev, double *dV, double *hess_V,
                            gradV_func_t dVdq, hessV_func_t hessian_V) {
    // ALPHA step
    symplectic_step_traj_tan(q, p, X, parameters, ALPHA * time_step, n_dev, dV,
                             hess_V, dVdq, hessian_V);
    // BETA step
    symplectic_step_traj_tan(q, p, X, parameters, BETA * time_step, n_dev, dV,
                             hess_V, dVdq, hessian_V);
    // ALPHA step
    symplectic_step_traj_tan(q, p, X, parameters, ALPHA * time_step, n_dev, dV,
                             hess_V, dVdq, hessian_V);
}
