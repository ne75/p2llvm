#include "observe.h"
#include <limits.h>

extern int __modsi3(int, int);

void test_body(void) {
    observe("positive_positive", __modsi3(100, 7));
    observe("negative_positive", __modsi3(-100, 7));
    observe("positive_negative", __modsi3(100, -7));
    observe("negative_negative", __modsi3(-100, -7));
    observe("zero_positive", __modsi3(0, 7));
    observe("zero_negative", __modsi3(0, -7));
    observe("exact", __modsi3(-98, 7));
    observe("unit_divisor", __modsi3(-123, 1));
    observe("negative_unit_divisor", __modsi3(123, -1));
    observe("smaller_dividend", __modsi3(-2, 7));
    observe("min_positive", __modsi3(INT_MIN, 7));
    observe("min_negative", __modsi3(INT_MIN, -7));
    observe("max_positive", __modsi3(INT_MAX, 7));
    observe("max_negative", __modsi3(INT_MAX, -7));
    observe("min_min", __modsi3(INT_MIN, INT_MIN));
    observe("max_min", __modsi3(INT_MAX, INT_MIN));
    observe("negative_min", __modsi3(-1, INT_MIN));
    observe("min_unit", __modsi3(INT_MIN, 1));
    // Division by zero and INT_MIN % -1 are undefined C operations.
}
