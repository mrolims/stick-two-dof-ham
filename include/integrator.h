#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "types.h"
#include <stddef.h>

void symplectic_step(double *q, double *p, const double *parameters,
                     const double time_step, double *dV, gradV_func_t dVdq);

void symplectic_step_traj_tan(double *q, double *p, double *X,
                              const double *parameters, double time_step,
                              size_t n_dev, double *dV, double *hess_V,
                              gradV_func_t dVdq, hessV_func_t hessian_V);

void yoshida4_step(double *q, double *p, const double *parameters,
                   const double time_step, double *dV, gradV_func_t dVdq);

void yoshida4_step_traj_tan(double *q, double *p, double *X,
                            const double *parameters, double time_step,
                            size_t n_dev, double *dV, double *hess_V,
                            gradV_func_t dVdq, hessV_func_t hessian_V);

#endif
