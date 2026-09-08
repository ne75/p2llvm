#include "observe.h"
#include <limits.h>
extern int helper_probe(int, int);
extern unsigned helper_bad_registers(void);

// Exact signed quotients, truncating toward zero. Undefined C divisions
// (zero divisor and INT_MIN / -1) are deliberately excluded.
static const struct { int a, b, q; } cases[] = {
    {100, 7, 14}, {-100, 7, -14}, {100, -7, -14}, {-100, -7, 14},
    {0, 7, 0}, {0, -7, 0}, {-98, 7, -14}, {-123, 1, -123},
    {123, -1, -123}, {-2, 7, 0}, {2, -7, 0},
    {INT_MIN, 7, -306783378}, {INT_MIN, -7, 306783378},
    {INT_MAX, 7, 306783378}, {INT_MAX, -7, -306783378},
    {INT_MIN, INT_MIN, 1}, {INT_MAX, INT_MIN, 0}, {-1, INT_MIN, 0},
    {INT_MIN, 1, INT_MIN}, {INT_MIN, 2, -1073741824},
    {INT_MIN, -2, 1073741824}, {INT_MAX, -1, -INT_MAX},
    {INT_MAX, INT_MAX, 1}, {INT_MIN, INT_MAX, -1}
};

void test_body(void) {
    unsigned bad = 0, registers = 0, first = ~0u;
    for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); ++i) {
        int q = helper_probe(cases[i].a, cases[i].b);
        registers |= helper_bad_registers();
        if (q != cases[i].q) { ++bad; if (first == ~0u) first = i; }
    }
    observe("division.cases", sizeof(cases) / sizeof(*cases));
    observe("division.bad", bad);
    observe("division.first_bad", first);
    observe("abi.bad_registers", registers);
}
