#define P2_TARGET_MHZ   300
#include <propeller.h>
#include <debug.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);

    brk(DEBUG_CODE_INIT);

    volatile int p = 57;
    dirh(p);

    while(1) {
        outnot(p);
    }

    return 0;
}