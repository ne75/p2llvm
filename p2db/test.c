#define P2_TARGET_MHZ   300
#include <propeller.h>
#include <debug.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

volatile unsigned stack[32];

__attribute__ ((cogmain)) void blink(void *p) {
    brk(DEBUG_CODE_INIT);

    int pin = ((int*)p)[0];
    dirh(pin);

    while(1) {
        outnot(pin);
    }
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);
    brk(DEBUG_CODE_INIT);

    hubset(DEBUG_INT_EN | DEBUG_COG0 | DEBUG_COG1);

    stack[0] = 60;
    _coginit(0x10, blink, (void*)stack);

    dirh(61);

    while(1) {
        outnot(61);
    }

    return 0;
}