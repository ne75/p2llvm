#include <propeller.h>
#include <stdio.h>
#define P2_TARGET_MHZ 300

#include <sys/p2es_clock.h>

volatile int64_t a = 1667857480399698;
volatile int64_t b = 1667857478892239;

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 2000000, 0);
    printf("$\n");
    
    volatile int64_t c = a - b;
    printf("%lld\n", c);

    printf("~\n");

    while (1) {
        waitx(500);
    }
}
