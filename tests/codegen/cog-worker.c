// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
// RUN: %clang -Os -mllvm -verify-machineinstrs -c %s -o %t.Os.o
struct mailbox { volatile unsigned stage, go, lock, blocked, acquired, input, value, high, id; };
extern int _locktry(unsigned);
extern void _unlock(unsigned);
void cog_worker(void *p) {
    struct mailbox *m = p;
    unsigned id;
    __asm__ volatile("cogid %0" : "=r"(id));
    m->id = id;
    m->blocked = _locktry(m->lock);
    m->stage = 1;
    while (!m->go) { }
    m->acquired = _locktry(m->lock);
    unsigned long long x = ((unsigned long long)m->input << 32) | 3;
    unsigned long long product = x*x;
    m->value = (unsigned)product;
    m->high = (unsigned)(product >> 32);
    _unlock(m->lock);
    m->stage = 2;
    __asm__ volatile("cogstop %0" : : "r"(id) : "memory");
    __builtin_unreachable();
}
