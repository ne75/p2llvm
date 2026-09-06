#include "observe.h"
extern unsigned long long pair(unsigned long long), memory(unsigned long long *, unsigned long long);
extern int select(int, int, int, int), call(int);
extern unsigned remainder(unsigned, unsigned);
int callee(int a, int b, int c, int d, int e) { return a + 2*b + 3*c + 4*d + 5*e; }
extern unsigned long long extended_sum(unsigned long long, unsigned long long);
void test_body(void) {
    OBSERVE64("carry.sum", extended_sum(0xffffffffULL, 1));
    OBSERVE64("pair", pair(0x8000000000000001ULL));
    observe("select.true", select(-1, 2, 19, 23));
    observe("select.false", select(3, 2, 19, 23));
    observe("remainder", remainder(100, 7));
    observe("call", call(7));
    unsigned long long value;
    OBSERVE64("memory", memory(&value, 0x0123456789abcdefULL));
}
