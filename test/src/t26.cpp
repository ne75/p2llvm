#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdlib.h>

#define RX_PIN 63
#define TX_PIN 62

unsigned stack[128];

void hub_cog(void *p) {
    printf("%d\n", (int)p);
    while(1);
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);
    waitx(CLKFREQ/5);
    printf("$\n"); // start of test character
    waitx(CLKFREQ/10);

    cogstart(hub_cog, 1, (int*)stack, sizeof(stack)/4);

    waitx(CLKFREQ/10);
    printf("~\n"); // end of test character

    while(1);

    return 0;
}