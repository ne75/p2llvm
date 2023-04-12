#define P2_TARGET_MHZ 200

#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 2000000, 0);

    printf("Hello World!\n");

    while(1) {
        waitx(CLKFREQ);
    }
}