#include "observe.h"
typedef unsigned long long u64;
extern u64 helper_probe(u64, u64);
extern unsigned helper_bad_registers(void);

// Low 64 product bits, including signed representations and unsigned overflow.
// Expected constants use arbitrary-precision integer multiplication modulo 2^64.
static const struct { u64 a, b, product; } cases[] = {
    {0x0000000000000000ULL, 0xffffffffffffffffULL, 0x0000000000000000ULL}, // 0
    {0x0000000000000001ULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL}, // 1
    {0xffffffffffffffffULL, 0xffffffffffffffffULL, 0x0000000000000001ULL}, // 2
    {0x8000000000000000ULL, 0x0000000000000001ULL, 0x8000000000000000ULL}, // 3
    {0x8000000000000000ULL, 0x0000000000000002ULL, 0x0000000000000000ULL}, // 4
    {0x8000000000000000ULL, 0xffffffffffffffffULL, 0x8000000000000000ULL}, // 5
    {0x7fffffffffffffffULL, 0x0000000000000002ULL, 0xfffffffffffffffeULL}, // 6
    {0x0000000100000000ULL, 0x0000000100000000ULL, 0x0000000000000000ULL}, // 7
    {0xffffffffffffffffULL, 0x0000000000000002ULL, 0xfffffffffffffffeULL}, // 8
    {0x00000000ffffffffULL, 0x00000000ffffffffULL, 0xfffffffe00000001ULL}, // 9
    {0x0000000100000001ULL, 0x0000000100000001ULL, 0x0000000200000001ULL}, // 10
    {0x0000000100000001ULL, 0x00000000ffffffffULL, 0xffffffffffffffffULL}, // 11
    {0x1234567887654321ULL, 0xfedcba9876543210ULL, 0xaf5fe1bd7a44a410ULL}, // 12
    {0x0000000000000003ULL, 0x0000000000000007ULL, 0x0000000000000015ULL}, // 13
    {0xfffffffffffffffdULL, 0x0000000000000007ULL, 0xffffffffffffffebULL}, // 14
    {0x0000000000000003ULL, 0xfffffffffffffff9ULL, 0xffffffffffffffebULL}, // 15
    {0xfffffffffffffffdULL, 0xfffffffffffffff9ULL, 0x0000000000000015ULL}, // 16
    {0x000000007fffffffULL, 0x0000000100000001ULL, 0x7fffffff7fffffffULL}, // 17
    {0xffffffff80000001ULL, 0x0000000100000001ULL, 0x8000000080000001ULL}, // 18
    {0x000000007fffffffULL, 0xfffffffeffffffffULL, 0x8000000080000001ULL}, // 19
    {0xffffffff80000001ULL, 0xfffffffeffffffffULL, 0x7fffffff7fffffffULL}, // 20
    {0x0000000100000001ULL, 0x0000000100000003ULL, 0x0000000400000003ULL}, // 21
    {0xfffffffeffffffffULL, 0x0000000100000003ULL, 0xfffffffbfffffffdULL}, // 22
    {0x0000000100000001ULL, 0xfffffffefffffffdULL, 0xfffffffbfffffffdULL}, // 23
    {0xfffffffeffffffffULL, 0xfffffffefffffffdULL, 0x0000000400000003ULL}, // 24
    {0x0000000123456789ULL, 0x0000000000abcdefULL, 0x00c379aaaa375de7ULL}, // 25
    {0xfffffffedcba9877ULL, 0x0000000000abcdefULL, 0xff3c865555c8a219ULL}, // 26
    {0x0000000123456789ULL, 0xffffffffff543211ULL, 0xff3c865555c8a219ULL}, // 27
    {0xfffffffedcba9877ULL, 0xffffffffff543211ULL, 0x00c379aaaa375de7ULL}, // 28
    {0x646fd0348e6dce82ULL, 0x50c16f5394cd1f00ULL, 0xc0ca38b6d21bbe00ULL}, // 29
    {0xae64a597d4e01d79ULL, 0xb02eae924edaf292ULL, 0x17ae94a93ab73102ULL}, // 30
    {0xd3ed8f491e825a5fULL, 0xd178d395ea77d473ULL, 0x7d23269a7f8e44adULL}, // 31
    {0x070fff541f855190ULL, 0xef22d7570e9cc8b7ULL, 0xafd4656643c5cdf0ULL}, // 32
    {0x1197a00b06287686ULL, 0x533d79304d1fee26ULL, 0x3d057b09306c2be4ULL}, // 33
    {0x18cd6bdcdd979f84ULL, 0x1314421500b428b9ULL, 0x637961abfc4ee664ULL}, // 34
    {0xdd7824cd4287426cULL, 0x593232a824985532ULL, 0x75be8bb48798d518ULL}, // 35
    {0x9e77720ea758f4bcULL, 0x74574123cbec2750ULL, 0x4f1ae1d98a651ec0ULL}, // 36
    {0x3b8003ed8863d989ULL, 0xec7bda92aa341d2fULL, 0x96c85dee83cd7527ULL}, // 37
    {0xf48b38bef2e66e3aULL, 0x6d31fd36e2d13d36ULL, 0x6adbc5d35639123cULL}, // 38
    {0xe37dfeb2bbae1c6bULL, 0xd57eff911328f729ULL, 0xcf5e2aa06e05ca23ULL}, // 39
    {0xe1d12936d2f9cbcfULL, 0x5927555bae80dbccULL, 0xb2be312d69e87df4ULL}, // 40
};

__attribute__((noinline)) static u64 pattern(u64 a, u64 b) { return a * b; }
void test_body(void) {
    unsigned bad = 0, patterns = 0, registers = 0, first = ~0u;
    for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); ++i) {
        u64 value = helper_probe(cases[i].a, cases[i].b);
        registers |= helper_bad_registers();
        if (value != cases[i].product) { ++bad; if (first == ~0u) first = i; }
        if (pattern(cases[i].a, cases[i].b) != cases[i].product) ++patterns;
    }
    observe("multiply.cases", sizeof(cases) / sizeof(*cases));
    observe("multiply.bad", bad);
    observe("multiply.pattern_bad", patterns);
    observe("multiply.first_bad", first);
    observe("abi.bad_registers", registers);
}
