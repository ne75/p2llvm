#include <stddef.h>

// Frozen comparison implementations from a00373c / production_baseline.
// These are test fixtures, not alternative runtime implementations.
#ifdef P2_BENCH_MEMSET
#define LUT __attribute__((section("lut"), cogtext, noinline, no_builtin("memset")))
LUT void *memset_previous(void *dst, int c, size_t n) {
    volatile unsigned char *p = dst;
    while (n--) *p++ = (unsigned char)c;
    return dst;
}

LUT void *memset_production_completed(void *dst, int c, size_t n) {
    // Original WRFAST/DJNZ body. Benchmarks only pass nonzero n.
    // Add the required drain so both timings include completed writes.
    asm volatile("wrfast #0, %[dst]\n"
                 ".Lproduction_fill: wfbyte %[c]\n"
                 "djnz %[n], #.Lproduction_fill\n"
                 "rdfast #0, %[dst]"
                 : [dst] "+r"(dst), [c] "+r"(c), [n] "+r"(n) : : "memory");
    return dst;
}

LUT void *memset_empty(void *dst, int c, size_t n) {
    asm volatile("" : : : "memory");
    return dst;
}
#else
__attribute__((noinline)) unsigned long long cnt64_previous(void) {
    unsigned low, high;
    asm volatile("getct %1 wc\ngetct %0" : "=r"(low), "=r"(high));
    return ((unsigned long long)high << 32) | low;
}

__attribute__((naked)) void counter_empty(void) {
    asm volatile("reta");
}
#endif
