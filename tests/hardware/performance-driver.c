#include "observe.h"
#include <stddef.h>

// GETCT measures sysclks; UART reporting happens outside every timed region.
static unsigned ticks(void) {
    unsigned value;
    asm volatile("getct %0" : "=r"(value) : : "memory");
    return value;
}

// Each observation is a batch of 64 calls, including common loop/call overhead.
// Empty-call timings are reported separately, not silently subtracted.
#ifdef P2_BENCH_MEMSET
extern void *memset(void *, int, size_t);
extern void *memset_previous(void *, int, size_t);
extern void *memset_production_completed(void *, int, size_t);
extern void *memset_empty(void *, int, size_t);
static unsigned char arena[1040] __attribute__((aligned(16)));
// Make each fill observable to the optimizer, including builtin memset.
// Without this barrier O2 can collapse 64 identical fills to one. The empty
// asm emits no instructions; apply it equally to every comparison path.
#define MEASURE(name, target) \
    __attribute__((noinline, no_builtin("memset"))) \
    static unsigned name(unsigned n) { \
        unsigned start = ticks(); \
        for (unsigned i = 0; i < 64; ++i) { \
            target(arena + 3, 0xa5, n); \
            asm volatile("" : : : "memory"); \
        } \
        return ticks() - start; \
    }
MEASURE(empty, memset_empty)
MEASURE(previous, memset_previous)
MEASURE(production, memset_production_completed)
MEASURE(updated, memset)
#else
extern unsigned long long _cnt64(void), cnt64_previous(void);
extern void counter_empty(void);
#define MEASURE(name, target) \
    __attribute__((noinline)) static unsigned name(unsigned unused) { \
        unsigned start = ticks(); \
        for (unsigned i = 0; i < 64; ++i) target(); \
        return ticks() - start; \
    }
MEASURE(empty, counter_empty)
MEASURE(previous, cnt64_previous)
MEASURE(updated, _cnt64)
#endif

void test_body(void) {
    unsigned (*const functions[])(unsigned) = {
        empty, previous,
#ifdef P2_BENCH_MEMSET
        production,
#endif
        updated
    };
    static const unsigned sizes[] = {
#ifdef P2_BENCH_MEMSET
        8, 64, 1024
#else
        0
#endif
    };
    // Alternate implementations for five samples to reduce order/phase bias.
    for (unsigned size = 0; size < sizeof(sizes) / sizeof(*sizes); ++size) {
        // Keep bookkeeping in memory, outside timed regions. This also avoids
        // unrolling the whole benchmark into a large chain of register selects.
        volatile unsigned minima[4] = {~0u, ~0u, ~0u, ~0u}, maxima[4] = {0, 0, 0, 0};
        for (unsigned sample = 0; sample < 5; ++sample) {
            for (unsigned f = 0; f < sizeof(functions) / sizeof(*functions); ++f) {
                unsigned cycles = functions[f](sizes[size]);
                if (cycles < minima[f]) minima[f] = cycles;
                if (cycles > maxima[f]) maxima[f] = cycles;
            }
        }
        for (unsigned f = 0; f < sizeof(functions) / sizeof(*functions); ++f) {
            // The numeric size index avoids depending on target formatting code.
            static char name[] = "cycles.0.0.min";
            name[7] = '0' + size; name[9] = '0' + f;
            name[11] = 'm'; name[12] = 'i'; name[13] = 'n';
            observe(name, minima[f]);
            name[11] = 'm'; name[12] = 'a'; name[13] = 'x';
            observe(name, maxima[f]);
        }
    }
    observe("batch.calls", 64);
    observe("batch.samples", 5);
}
