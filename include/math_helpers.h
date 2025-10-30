
#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H

#include <stddef.h>

static inline int sgn(double x) {

    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

#endif
