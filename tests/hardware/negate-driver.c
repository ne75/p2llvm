#include "observe.h"
typedef unsigned long long u64;
extern u64 helper_probe(u64);
extern unsigned helper_bad_registers(void);

// Exact (-input) modulo 2^64, computed with arbitrary-precision integers.
// The helper's INT64_MIN wrap is tested directly and through unsigned C only;
// signed C negation excludes INT64_MIN because that expression overflows.
static const struct { u64 input, expected; } cases[] = {
    {0x0000000000000000ULL, 0x0000000000000000ULL}, // 0
    {0x0000000000000001ULL, 0xffffffffffffffffULL}, // 1
    {0x0000000000000002ULL, 0xfffffffffffffffeULL}, // 2
    {0x0000000000000003ULL, 0xfffffffffffffffdULL}, // 3
    {0x000000007fffffffULL, 0xffffffff80000001ULL}, // 4
    {0x0000000080000000ULL, 0xffffffff80000000ULL}, // 5
    {0x00000000fffffffeULL, 0xffffffff00000002ULL}, // 6
    {0x00000000ffffffffULL, 0xffffffff00000001ULL}, // 7
    {0x0000000100000000ULL, 0xffffffff00000000ULL}, // 8
    {0x0000000100000001ULL, 0xfffffffeffffffffULL}, // 9
    {0x0000000100000002ULL, 0xfffffffefffffffeULL}, // 10
    {0x00000001ffffffffULL, 0xfffffffe00000001ULL}, // 11
    {0x7fffffff00000000ULL, 0x8000000100000000ULL}, // 12
    {0x7ffffffffffffffeULL, 0x8000000000000002ULL}, // 13
    {0x7fffffffffffffffULL, 0x8000000000000001ULL}, // 14
    {0x8000000000000000ULL, 0x8000000000000000ULL}, // 15
    {0x8000000000000001ULL, 0x7fffffffffffffffULL}, // 16
    {0xffffffff00000000ULL, 0x0000000100000000ULL}, // 17
    {0xffffffff00000001ULL, 0x00000000ffffffffULL}, // 18
    {0xfffffffffffffffeULL, 0x0000000000000002ULL}, // 19
    {0xffffffffffffffffULL, 0x0000000000000001ULL}, // 20
    {0x0000000200000001ULL, 0xfffffffdffffffffULL}, // 21
    {0xaaaaaaaa55555555ULL, 0x55555555aaaaaaabULL}, // 22
    {0x55555555aaaaaaaaULL, 0xaaaaaaaa55555556ULL}, // 23
    {0x0123456789abcdefULL, 0xfedcba9876543211ULL}, // 24
    {0xfedcba9876543210ULL, 0x0123456789abcdf0ULL}, // 25
    {0x1741c5ac970b9b07ULL, 0xe8be3a5368f464f9ULL}, // 26
    {0x7732f5e6360d6440ULL, 0x88cd0a19c9f29bc0ULL}, // 27
    {0x74d510804bfa2e9bULL, 0x8b2aef7fb405d165ULL}, // 28
    {0x58d8f9ad27693682ULL, 0xa7270652d896c97eULL}, // 29
    {0xe55a6c9c9031e8e7ULL, 0x1aa593636fce1719ULL}, // 30
    {0x5eac44cd02c97920ULL, 0xa153bb32fd3686e0ULL}, // 31
};

__attribute__((noinline)) u64 negate_unsigned(u64 a) { return -a; }
__attribute__((noinline)) long long negate_signed(long long a) { return -a; }

void test_body(void) {
    unsigned bad = 0, unsigned_bad = 0, signed_bad = 0, signed_cases = 0;
    unsigned registers = 0, first = ~0u;
    for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); ++i) {
        u64 input = cases[i].input, expected = cases[i].expected;
        unsigned helper_wrong = helper_probe(input) != expected;
        registers |= helper_bad_registers();
        unsigned unsigned_wrong = negate_unsigned(input) != expected;
        unsigned signed_wrong = 0;
        if (input != 0x8000000000000000ULL) {
            signed_wrong = (u64)negate_signed((long long)input) != expected;
            ++signed_cases;
        }
        bad += helper_wrong;
        unsigned_bad += unsigned_wrong;
        signed_bad += signed_wrong;
        if (first == ~0u && (helper_wrong || unsigned_wrong || signed_wrong))
            first = i;
    }
    observe("negate.cases", sizeof(cases) / sizeof(*cases));
    observe("negate.signed_cases", signed_cases);
    observe("negate.bad", bad);
    observe("negate.unsigned_bad", unsigned_bad);
    observe("negate.signed_bad", signed_bad);
    observe("negate.first_bad", first);
    observe("abi.bad_registers", registers);
}
