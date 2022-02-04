#include <stdio.h>

#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);

    printf("$\n"); // start of test character

    printf("test\n");
    // int x;

    // asm(
    //     "mov r0, #10\n"
    //     "alts r0, #2\n"
    //     "mov r1, $0\n"
    // : "=r"(x) : : "r0");
    
    // printf("x after alts: %d\n", x);

    printf("~\n"); // end of test character
    return 0;
}