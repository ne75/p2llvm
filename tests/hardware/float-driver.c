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
}
