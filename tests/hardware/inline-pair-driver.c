#include "observe.h"
extern unsigned long long pair_constant(void);
extern unsigned long long pair_add(unsigned long long, unsigned long long);

void test_body(void) {
    OBSERVE64("pair.constant", pair_constant());
    OBSERVE64("pair.carry", pair_add(0xffffffffULL, 1));
    OBSERVE64("pair.wrap", pair_add(~0ULL, 1));
    OBSERVE64("pair.high", pair_add(0x100000002ULL, 0x300000004ULL));
    OBSERVE64("pair.same", pair_add(0x87654321abcdef01ULL, 0x87654321abcdef01ULL));
}
