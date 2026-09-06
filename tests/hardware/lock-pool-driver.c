#include "observe.h"
extern unsigned _locknew(void);
extern void _lockret(unsigned);

// Use only IDs returned by LOCKNEW, never an assumed free lock number.
static unsigned take_release_return(unsigned id) {
    unsigned taken;
#define LOCK_CASE(N) case N: \
    __asm__ volatile("locktry #" #N " wc\nwrc %0\n" \
                     "lockrel #" #N "\nlockret #" #N \
                     : "=r"(taken) : : "memory"); return taken;
    switch (id) {
        LOCK_CASE(0)  LOCK_CASE(1)  LOCK_CASE(2)  LOCK_CASE(3)
        LOCK_CASE(4)  LOCK_CASE(5)  LOCK_CASE(6)  LOCK_CASE(7)
        LOCK_CASE(8)  LOCK_CASE(9)  LOCK_CASE(10) LOCK_CASE(11)
        LOCK_CASE(12) LOCK_CASE(13) LOCK_CASE(14) LOCK_CASE(15)
    }
#undef LOCK_CASE
    return 0;
}

void test_body(void) {
    unsigned ids[16], count = 0, mask = 0, unique = 1, failed = 0;
    for (unsigned i = 0; i < 17; ++i) {
        unsigned id = _locknew();
        if (id >= 16) { failed = id; break; }
        if (mask & (1u << id)) { unique = 0; break; }
        mask |= 1u << id;
        ids[count++] = id;
    }
    unsigned taken = 0;
    for (unsigned i = 0; i < count; ++i)
        taken += take_release_return(ids[i]);
    unsigned reused = _locknew();
    unsigned reusable = reused < 16 && (mask & (1u << reused)) != 0;
    if (reused < 16) _lockret(reused);
    // This firmware's startup reserves two of the sixteen locks.
    observe("pool.available", count);
    observe("pool.unique", unique);
    observe("pool.exhausted", failed);
    observe("pool.immediate_taken", taken);
    observe("pool.reusable", reusable);
}
