#include "observe.h"
struct mailbox { volatile unsigned stage, go, lock, blocked, acquired, input, value, high, id; };
extern void cog_worker(void *);
extern int cogstart(void (*)(void *), int, int *, unsigned), _locktry(unsigned);
extern unsigned _locknew(void);
extern void _unlock(unsigned), _lockret(unsigned);
static struct mailbox m;
static int stack[512];
void test_body(void) {
    unsigned self;
    __asm__ volatile("cogid %0" : "=r"(self));
    m.lock = _locknew();
    observe("lock.allocated", m.lock < 16);
    if (m.lock >= 16) return;
    observe("lock.first", _locktry(m.lock));
    m.input = 50000;
    int cog = cogstart(cog_worker, (int)&m, stack, sizeof stack);
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
}
