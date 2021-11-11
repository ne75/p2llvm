#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"
#include <debug.h>

#include <stdio.h>
#include <stdlib.h>

#define RX_PIN 63
#define TX_PIN 62

unsigned stack[128];

void hub_cog(void *p) {
    printf("%d\n", (int)p);
    busywait();
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    waitx(CLKFREQ/5);
    printf("$\n"); // start of test character
    waitx(CLKFREQ/10);

    cogstart(hub_cog, 1, (int*)stack, sizeof(stack));

    waitx(CLKFREQ/5);
    printf("~\n"); // end of test character

    busywait();

    return 0;
}