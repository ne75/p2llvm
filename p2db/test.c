#define P2_TARGET_MHZ   300
#include <propeller.h>
#include <debug.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

unsigned stack[32];

void blink(void *p) {
    brk(DEBUG_CODE_INIT);

    dirh(60);

    while(1) {
        outnot(60);
    }
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);
    brk(DEBUG_CODE_INIT);

    // hubset(DEBUG_INT_EN | DEBUG_COG0);

    // cogstart(blink, NULL, (int*)stack, sizeof(stack));

    dirh(61);

    while(1) {
        outnot(61);
    }

    return 0;
}