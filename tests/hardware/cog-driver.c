#include "observe.h"
struct mailbox { volatile unsigned stage, go, lock, blocked, acquired, input, value, high, id; };
extern void cog_worker(void *);
extern int cogstart(void (*)(void *), int, int *, unsigned), _locktry(unsigned);
extern unsigned _locknew(void);
extern void _unlock(unsigned), _lockret(unsigned);
static struct mailbox m;
static int stack[512];
#ifdef P2_INIT_FORM
extern void __start(void);
static int start_variant(void) {
    unsigned failed, mode = 16;
    stack[0] = (int)cog_worker;
    stack[1] = (int)&m;
#if P2_INIT_FORM == 1
    __asm__ volatile("setq %1\ncoginit #16, ##__start wc\nwrc %0"
                     : "=r"(failed) : "r"(stack) : "memory");
#elif P2_INIT_FORM == 2
    __asm__ volatile("setq %1\ncoginit #16, %2 wc\nwrc %0"
                     : "=r"(failed) : "r"(stack), "r"(__start) : "memory");
#else
    __asm__ volatile("setq %2\ncoginit %1, ##__start wc\nwrc %0"
                     : "=r"(failed), "+r"(mode) : "r"(stack) : "memory");
#endif
    return -(int)failed;
}
static void stop_variant(unsigned id) {
#define STOP_CASE(N) case N: __asm__ volatile("cogstop #" #N : : : "memory"); break;
    switch (id) {
        STOP_CASE(0) STOP_CASE(1) STOP_CASE(2) STOP_CASE(3)
        STOP_CASE(4) STOP_CASE(5) STOP_CASE(6) STOP_CASE(7)
    }
#undef STOP_CASE
}
#endif
void test_body(void) {
    unsigned self;
    __asm__ volatile("cogid %0" : "=r"(self));
    m.lock = _locknew();
    observe("lock.allocated", m.lock < 16);
    if (m.lock >= 16) return;
    observe("lock.first", _locktry(m.lock));
    m.input = 50000;
#ifdef P2_INIT_FORM
    int cog = start_variant();
#else
    int cog = cogstart(cog_worker, (int)&m, stack, sizeof stack);
#endif
    observe("cog.started", cog >= 0);
    if (cog < 0) { _unlock(m.lock); _lockret(m.lock); return; }
    // The host's hard deadline covers failed startup and either handshake.
    while (m.stage != 1) { }
    observe("cog.distinct", m.id != self);
    observe("lock.other_blocked", m.blocked);
    _unlock(m.lock);
    m.go = 1;
    while (m.stage != 2) { }
    observe("lock.after_release", m.acquired);
    observe("cog.value.lo", m.value);
    observe("cog.value.hi", m.high);
    observe("lock.after_worker", _locktry(m.lock));
    _unlock(m.lock);
    _lockret(m.lock);
#ifdef P2_INIT_FORM
    if (m.id < 8 && m.id != self) stop_variant(m.id);
    unsigned running;
    __asm__ volatile("cogid %1 wc\nwrc %0" : "=r"(running) : "r"(m.id));
    observe("cog.after_stop", running);
#endif
}
