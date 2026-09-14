#include "basics.h"
#include <string.h>
#define NI __attribute__((noinline))
static volatile u32 divisor32 = 13;
static volatile u64 multiplier = 0x1234567800000003ULL, divisor64 = 3;
static unsigned char source[1040] __attribute__((aligned(16)));
static unsigned char destination[1040] __attribute__((aligned(16)));

void basic_prepare(void) {
    volatile unsigned char *s = source, *d = destination;
    for (unsigned i = 0; i < sizeof(source); ++i) {
        s[i] = (unsigned char)(i * 37 + 11);
        d[i] = 0xa5;
    }
}

NI u32 basic_empty(u32 n) { return n; }
NI u32 basic_alu(u32 n) {
    for (unsigned i = 0; i < 16; ++i) {
        n = n * 1664525u + 1013904223u;
        n = (n << 7) | (n >> 25);
        n ^= n >> 11;
    }
    return n;
}
NI u32 basic_div32(u32 n) { return ((int)(n * 12345u + 7u) - 100000) / (int)divisor32; }
NI u32 basic_mul64(u32 n) {
    u64 result = (0x7654321000000000ULL | n) * multiplier;
    return (u32)result ^ (u32)(result >> 32);
}
NI u32 basic_div64(u32 n) {
    u64 a = 0x1234567800000000ULL | n, b = divisor64;
    u64 q = a / b, rem = a % b;
    return (u32)q ^ (u32)(q >> 32) ^ (u32)rem;
}
NI u32 basic_single(u32 n) { float f = (float)(n + 1); return (u32)(f * 1.5f + 2.0f); }
NI u32 basic_minimum(u32 n) {
    u32 a = n * 17, b = 700 - n * 3;
    return a > b ? b : a;
}

// Include write completion equally in both toolchains' memory measurements.
// RDFast runs from LUT so it cannot replace the executing HUB instruction FIFO.
#ifdef __propeller2__
__attribute__((section("lut"), cogtext, noinline))
static void complete_writes(void) { __asm__ volatile("rdfast #0, #0" : : : "memory"); }
#else
static void complete_writes(void) {}
#endif
#define COPY(name, len, dst, src) \
    NI u32 basic_##name(u32 n) { \
        memcpy(destination + dst, source + src, len); \
        return n ^ destination[dst] ^ ((u32)destination[dst + len - 1] << 8); \
    }
COPY(copy64, 64, 4, 4)
COPY(copy1024, 1024, 4, 4)
COPY(copy_unaligned, 64, 3, 1)
#define FILL(name, len) \
    NI u32 basic_##name(u32 n) { \
        memset(destination + 3, (int)n, len); \
        complete_writes(); \
        return n ^ destination[3] ^ ((u32)destination[3 + len - 1] << 8); \
    }
FILL(fill8, 8)
FILL(fill64, 64)
FILL(fill1024, 1024)

u32 basic_check_memory(unsigned id, u32 n) {
    if (id < CASE_copy64 || id > CASE_fill1024) return 0;
    unsigned size = (id == CASE_copy1024 || id == CASE_fill1024) ? 1024 : (id == CASE_fill8 ? 8 : 64);
    unsigned offset = id < CASE_copy_unaligned ? 4 : 3;
    volatile unsigned char *actual = destination;
    unsigned bad = 0;
    for (unsigned i = 0; i < sizeof(destination); ++i) {
        unsigned expected = 0xa5;
        if (i >= offset && i < offset + size)
            expected = id <= CASE_copy_unaligned ? ((i - offset + (id == CASE_copy_unaligned ? 1 : 4)) * 37 + 11) & 255 : n & 255;
        bad += actual[i] != expected;
    }
    return bad;
}
NI static u32 stack_sum(volatile u32 *values) {
    u32 sum = 0;
    for (unsigned i = 0; i < 16; ++i) sum += values[i];
    return sum;
}
NI u32 basic_stack(u32 n) {
    volatile u32 values[16];
    for (unsigned i = 0; i < 16; ++i) values[i] = n + i;
    return stack_sum(values) ^ values[n & 15];
}
