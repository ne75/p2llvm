#include <stdio.h>
#include <stdint.h>
#include <debug.h>
#include <math.h>

#include <stdarg.h>

union float_rep {
    float f;
    unsigned int i;
};

