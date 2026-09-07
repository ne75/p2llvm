#include "observe.h"
typedef __SIZE_TYPE__ size_t;
extern void *memset(void *, int, size_t);

// Read while still in LUT, before returning to HUB can reconfigure the FIFO.
#ifndef P2_TEST_HOST
__attribute__((section("lut"), cogtext))
#endif
__attribute__((noinline, no_builtin("memset")))
static unsigned lut_readback(unsigned char *p, unsigned n, int value) {
    memset(p, value, n);
    return ((volatile unsigned char *)p)[n - 1];
}

__attribute__((no_builtin("memset")))
void test_body(void) {
    static const unsigned sizes[] = {0, 1, 2, 3, 4, 7, 15, 16, 17, 31, 32, 33,
                                     63, 64, 65, 127, 128, 129, 511, 512, 513, 1023, 1024};
    static const int values[] = {0, 0x80, 0xff, 0x1234, -1};
    volatile unsigned char arena[1040];
    unsigned cases = 0, bad_bytes = 0, bad_returns = 0, first_bad = ~0u;
    for (unsigned offset = 0; offset < 4; ++offset) {
        for (unsigned length = 0; length < sizeof(sizes) / sizeof(*sizes); ++length) {
            for (unsigned v = 0; v < sizeof(values) / sizeof(*values); ++v) {
                for (unsigned i = 0; i < sizeof(arena); ++i) arena[i] = 0x5a;
                unsigned start = 8 + offset, n = sizes[length];
                void *p = (unsigned char *)arena + start;
                bad_returns += memset(p, values[v], n) != p;
                for (unsigned i = 0; i < sizeof(arena); ++i) {
                    unsigned expected = i >= start && i < start + n ?
                                        (unsigned char)values[v] : 0x5a;
                    if (arena[i] != expected) {
                        ++bad_bytes;
                        if (first_bad == ~0u) first_bad = cases;
                    }
                }
                ++cases;
            }
        }
    }
    observe("memset.cases", cases);
    observe("memset.bad_bytes", bad_bytes);
    observe("memset.bad_returns", bad_returns);
    observe("memset.first_bad", first_bad);
    observe("memset.lut_readback", lut_readback((unsigned char *)arena + 3, 129, 0x1234));
}
