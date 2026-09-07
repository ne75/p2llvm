#include "observe.h"
extern unsigned long long _cnt64(void);

static unsigned reference_low(void) {
    unsigned low;
    asm volatile("getct %0" : "=r"(low));
    return low;
}

void test_body(void) {
    // Separate, opt-in long test. CT is cleared on reset. Wait independently of
    // _cnt64, then sample on either side of the first 32-bit wrap.
    while (reference_low() < 0xffff0000u) {}
    unsigned long long before = _cnt64();
    while (reference_low() >= 0xffff0000u) {}
    unsigned long long after = _cnt64();
    observe("rollover.before.high", (unsigned)(before >> 32));
    observe("rollover.after.high", (unsigned)(after >> 32));
    OBSERVE64("rollover.elapsed", after - before);
}
