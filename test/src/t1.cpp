#include <stdio.h>

#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 2000000);

    printf("$\n"); // start of test character
    printf("Hello\nWorld!\n");
    printf("~\n"); // end of test character

    return 0;
}