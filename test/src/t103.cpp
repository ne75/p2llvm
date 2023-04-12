#include <propeller.h>
#include <stdio.h>
#define P2_TARGET_MHZ 300

#include <sys/p2es_clock.h>

volatile bool locked = true;
volatile int64_t offset = 12345;

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 2000000, 0);
    printf("$\n");
    
    if (locked) {
        if (offset > 50000 || offset < -50000) {
            locked = false;
            printf("unlock\n");
        } else {
            printf("keep lock and adjust\n");
        }
    } else {
        printf("set clock\n");
        if (offset < 10000 && offset > -10000) {
            locked = true;
            printf("lock\n");
        }
    }

    printf("~\n");

    while (1) {
        waitx(500);
    }
}
