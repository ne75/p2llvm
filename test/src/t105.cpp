#include <propeller.h>
#include <stdio.h>
#define P2_TARGET_MHZ 300

#include <sys/p2es_clock.h>

// #define qrotate_bad(x, y, angle) asm volatile ("setq %0\nqrotate %1, %2\n" : : "ri"(y), "ri"(x), "ri"(angle))

int main() {
    // _clkset(_SETFREQ, _CLOCKFREQ);
    // _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 2000000, 0);

    asm volatile (
        "setword r1, r2, #0"
    );
    // printf("$\n");

    // qrotate_bad(x, y, a);
    // int a1, a2;
    // getqx(a1);
    // getqy(a2);

    // printf("%d %d\n", a1, a2);

    // printf("~\n");

    // while (1) {
    //     waitx(500);
    // }
}
