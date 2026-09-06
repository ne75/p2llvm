#include "observe.h"
extern unsigned _cnt(void);
extern unsigned long long _cnt64(void);
void test_body(void) {
    unsigned long long first = _cnt64();
    __asm__ volatile("waitx #64");
    unsigned long long last = _cnt64();
    OBSERVE64("counter64.elapsed", last-first);
    unsigned a = _cnt(), delay = 97;
    __asm__ volatile("waitx %0" : : "r"(delay));
    observe("counter32.elapsed", _cnt()-a);
}
