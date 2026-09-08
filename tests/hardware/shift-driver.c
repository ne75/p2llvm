#include "observe.h"
#include "shift-reference.h"
typedef unsigned long long u64;
union words { u64 all; struct shift_words halves; };
extern u64 helper_probe(u64, unsigned);
extern unsigned helper_bad_registers(void);

#if defined(P2_ASHLDI)
#define KIND 0
__attribute__((noinline)) u64 shift_pattern(u64 a, unsigned n) { return a << n; }
#elif defined(P2_LSHRDI)
#define KIND 1
__attribute__((noinline)) u64 shift_pattern(u64 a, unsigned n) { return a >> n; }
#elif defined(P2_ASHRDI)
#define KIND 2
__attribute__((noinline)) u64 shift_pattern(u64 a, unsigned n) { return (long long)a >> n; }
#else
#error Select a shift helper
#endif

// Include distinct halves so a zero-count OR cannot accidentally be invisible.
static const u64 edges[] = {
    0, ~0ULL, 0x0000000100000002ULL, 0x8000000000000000ULL,
    0x7fffffffffffffffULL, 0xaaaaaaaa55555555ULL,
    0x0123456789abcdefULL, 0xfedcba9876543210ULL
};

void test_body(void) {
    unsigned bad = 0, patterns = 0, registers = 0, first = ~0u, total = 0;
    union words actual = {0}, expected = {0};
    // Eight edge values, every single set bit, and every single cleared bit.
    for (unsigned i = 0; i < 136; ++i) {
        union words input;
        if (i < 8) input.all = edges[i];
        else {
            unsigned bit = (i - 8) & 63;
            input.halves.lo = bit < 32 ? 1u << bit : 0;
            input.halves.hi = bit >= 32 ? 1u << (bit - 32) : 0;
            if (i >= 72) {
                input.halves.lo = ~input.halves.lo;
                input.halves.hi = ~input.halves.hi;
            }
        }
        for (unsigned n = 0; n < 64; ++n) {
            union words want = {.halves = shift_reference(input.halves, n, KIND)};
            u64 got = helper_probe(input.all, n);
            registers |= helper_bad_registers();
            u64 via_c = shift_pattern(input.all, n);
            if (got != want.all) ++bad;
            if (via_c != want.all) ++patterns;
            if (first == ~0u && (got != want.all || via_c != want.all)) {
                first = total;
                actual.all = got != want.all ? got : via_c;
                expected = want;
            }
            ++total;
        }
    }
    observe("shift.cases", total);
    observe("shift.bad", bad);
    observe("shift.pattern_bad", patterns);
    observe("shift.first_bad", first); // input index * 64 + count
    observe("shift.actual.lo", actual.halves.lo);
    observe("shift.actual.hi", actual.halves.hi);
    observe("shift.expected.lo", expected.halves.lo);
    observe("shift.expected.hi", expected.halves.hi);
    observe("abi.bad_registers", registers);
}
