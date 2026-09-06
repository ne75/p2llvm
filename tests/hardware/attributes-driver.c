#include "observe.h"
extern unsigned attr_text(unsigned), attr_cache(unsigned), attr_entry(void);
void test_body(void) {
    unsigned entry;
#ifdef P2_TEST_HOST
    entry = attr_entry();
#else
    // cogmain deliberately has no callee-save prologue. This known leaf only
    // touches R0 and R31; declare both clobbers for the HUB test adapter.
    __asm__ volatile("calla #\\attr_entry\nmov %0, r31"
                     : "=r"(entry) : : "r0", "r31", "memory");
#endif
    observe("attribute.entry", entry);
    observe("attribute.text", attr_text(13));
    observe("attribute.cache", attr_cache(0x5678));
}
