#include "basics.h"
#include "observe.h"

struct Case { u32 (*run)(u32); const char *hash, *memory, *sum, *cycles[5]; };
#define ENTRY(n) {basic_##n, #n ".hash", #n ".memory", #n ".sum", \
    {#n ".cycles0", #n ".cycles1", #n ".cycles2", #n ".cycles3", #n ".cycles4"}},
static const struct Case cases[] = { BASIC_CASES(ENTRY) };

static u32 counter(void) {
    u32 value = 0;
#ifndef P2_TEST_HOST
    __asm__ volatile("getct %0" : "=r"(value) : : "memory");
#endif
    return value;
}

void test_body(void) {
#ifdef P2_TEST_HOST
    observe("clock_hz", 0);
#else
    observe("clock_hz", *(volatile u32 *)0x14);
#endif
    for (unsigned id = 0; id < sizeof(cases) / sizeof(cases[0]); ++id) {
        const struct Case *item = &cases[id];
        u32 hash = 2166136261u, bad = 0, sum = 0, cycles[5];
        for (u32 n = 0; n < 64; ++n) {
            basic_prepare();
            hash = (hash ^ item->run(n)) * 16777619u;
            bad += basic_check_memory(id, n);
        }
        // One warmup; no serial output or full-buffer validation in the timer.
        basic_prepare();
        item->run(0);
        for (unsigned sample = 0; sample < 5; ++sample) {
            u32 total = 0, start = counter();
            for (u32 n = 0; n < 64; ++n) {
                total += item->run(n);
                __asm__ volatile("" : "+r"(total) : : "memory");
            }
            cycles[sample] = counter() - start;
            sum += total;
        }
        observe(item->hash, hash);
        observe(item->memory, bad);
        observe(item->sum, sum);
        for (unsigned sample = 0; sample < 5; ++sample)
            observe(item->cycles[sample], cycles[sample]);
    }
}
