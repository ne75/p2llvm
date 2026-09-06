// This program reports observations; only the host decides whether they pass.
#include "observe.h"
#ifdef P2_TEST_HOST
#include <stdio.h>
static void put(unsigned char c) { putchar(c); }
#else
// Keep the transport small and independently reviewable. No printf, malloc,
// formatter, locks, or expected-result comparisons execute on the chip.
extern void _uart_init(unsigned, unsigned, unsigned, unsigned);
extern void _uart_putc(unsigned char, int);
static void put(unsigned char c) { _uart_putc(c, 62); }
#endif

static unsigned count;
static void text(const char *p) { while (*p) put(*p++); }
static void hex(unsigned value) {
    for (unsigned i = 0; i < 8; ++i) {
        unsigned digit = value >> 28;
        put(digit < 10 ? '0' + digit : 'a' + digit - 10);
        value <<= 4;
    }
}
void observe(const char *name, unsigned value) {
    text("P2VALUE "); text(name); put(' '); hex(value); put(' ');
    hex(~value); put('\n'); ++count;
}
int main(void) {
#ifndef P2_TEST_HOST
    _uart_init(63, 62, P2_TEST_BAUD, 0);
#endif
    text("P2TEST " P2_TEST_RUN_ID "\n");
    observe("transport.zero", 0);
    observe("transport.ones", 0xffffffffu);
    observe("transport.alternating", 0xa55a5aa5u);
    test_body();
    text("P2END " P2_TEST_RUN_ID " "); hex(count); put('\n');
#ifndef P2_TEST_HOST
    // loadp2 -q terminal exit sequence. A zero exit is not a test verdict.
    put(255); put(0); put(0);
#endif
    return 0;
}
