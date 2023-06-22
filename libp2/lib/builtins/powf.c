#define SINGLE_PRECISION
#include "fp_lib.h"

extern fp_t __powf(fp_t a, fp_t _b);

COMPILER_RT_ABI fp_t powf(fp_t a, fp_t b) {
    return __powf(a, b);
}
