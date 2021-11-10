#define P2_TARGET_MHZ   300
#include <propeller.h>
#include <debug.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

unsigned stack1[32];
unsigned stack2[32];
unsigned stack3[32];

void blink(void *p) {
    brk(DEBUG_CODE_INIT);

    int pin = (int)p;
    dirh(pin);

    while(1) {
        outnot(pin);
    }
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(DBG_UART_RX_PIN, DBG_UART_TX_PIN, 3000000);
    brk(DEBUG_CODE_INIT);

    hubset(DEBUG_INT_EN | DEBUG_COG0 | DEBUG_COG1 | DEBUG_COG2 | DEBUG_COG3);

    cogstart(blink, 58, (int*)stack1, sizeof(stack1));
    cogstart(blink, 59, (int*)stack2, sizeof(stack2));
    cogstart(blink, 60, (int*)stack3, sizeof(stack3));

    dirh(61);

    while(1) {
        outnot(61);
    }

    return 0;
}