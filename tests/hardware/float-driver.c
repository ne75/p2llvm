#include "observe.h"
extern float single(float, float);
extern double twice(double, double), convert_back(long long);
extern int compare_double(double, double);
extern long long convert(double);
void test_body(void) {
    union { float f; unsigned u; } f = {single(6.0f, 2.0f)};
    union { double f; unsigned long long u; } d = {twice(6.0, 2.0)};
    observe("float.operations", f.u);
    OBSERVE64("double.operations", d.u);
    observe("double.compare", compare_double(-1.5, 2.0));
    OBSERVE64("double.to_int", convert(-123.75));
    d.f = convert_back(-123);
    OBSERVE64("int.to_double", d.u);
    d.f = convert_back(0);
    OBSERVE64("int.zero", d.u);
    d.f = convert_back(-1);
    OBSERVE64("int.negative", d.u);
    // Adjacent ties around 2^53 must round to the even significand.
    d.f = convert_back(9007199254740993LL);
    OBSERVE64("int.tie_down", d.u);
    d.f = convert_back(9007199254740995LL);
    OBSERVE64("int.tie_up", d.u);
    d.f = convert_back(9223372036854775807LL);
    OBSERVE64("int.max", d.u);
    d.f = convert_back(-9223372036854775807LL);
    OBSERVE64("int.negative_max", d.u);
}
