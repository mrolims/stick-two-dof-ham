#ifndef CHAOTIC_INDICATORS_H
#define CHAOTIC_INDICATORS_H

#include "types.h"
#include <stddef.h>

double calculate_mle(double *q, double *p, const double *parameters,
                     double total_time, double time_step, size_t rescale_every,
                     gradV_func_t dVdq, hessV_func_t hessianV);
#endif
