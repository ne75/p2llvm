#include "observe.h"
#include "shift-reference.h"
typedef unsigned long long u64;
union words { u64 all; struct shift_words halves; };
extern u64 helper_probe(u64, unsigned);
extern unsigned helper_bad_registers(void);

// Extract actual return words without lowering a C i64 >> 32 or split store
// through __lshrdi3. Pair word printing has its own executable fixture.
static struct shift_words unpack(u64 value) {
    struct shift_words out;
    __asm__("mov %0, %L2\nmov %1, %H2"
            : "=&r"(out.lo), "=&r"(out.hi) : "r"(value));
    return out;
}

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
static const struct shift_words edges[] = {
    {0, 0}, {~0u, ~0u}, {2, 1}, {0, 0x80000000},
    {0xffffffff, 0x7fffffff}, {0x55555555, 0xaaaaaaaa},
    {0x89abcdef, 0x01234567}, {0x76543210, 0xfedcba98}
};

void test_body(void) {
    unsigned bad = 0, patterns = 0, registers = 0, first = ~0u, total = 0;
    struct shift_words actual = {0, 0}, expected = {0, 0};
    // Eight edge values, every single set bit, and every single cleared bit.
    for (unsigned i = 0; i < 136; ++i) {
        // Memory word accesses prevent i64 pack/unpack lowering from calling
        // the shift helpers under test while preparing inputs or expectations.
        volatile union words input;
        if (i < 8) {
            input.halves.lo = edges[i].lo;
            input.halves.hi = edges[i].hi;
        }
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
            struct shift_words start = {input.halves.lo, input.halves.hi};
            struct shift_words want = shift_reference(start, n, KIND);
            struct shift_words got = unpack(helper_probe(input.all, n));
            registers |= helper_bad_registers();
            struct shift_words via_c = unpack(shift_pattern(input.all, n));
            unsigned bad_helper = got.lo != want.lo || got.hi != want.hi;
            unsigned bad_pattern = via_c.lo != want.lo || via_c.hi != want.hi;
            bad += bad_helper;
            patterns += bad_pattern;
            if (first == ~0u && (bad_helper || bad_pattern)) {
                first = total;
                actual = bad_helper ? got : via_c;
                expected = want;
            }
            ++total;
        }
    }
    observe("shift.cases", total);
    observe("shift.bad", bad);
    observe("shift.pattern_bad", patterns);
    observe("shift.first_bad", first); // input index * 64 + count
    observe("shift.actual.lo", actual.lo);
    observe("shift.actual.hi", actual.hi);
    observe("shift.expected.lo", expected.lo);
    observe("shift.expected.hi", expected.hi);
    observe("abi.bad_registers", registers);
}
