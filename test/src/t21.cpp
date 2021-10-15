#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

void recurse(int i) {
    if (i == 0) return;

    printf("%d\n", i);

    recurse(i-1);
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);
    printf("$\n"); // start of test character

    recurse(10);

    printf("~\n"); // end of test character
    return 0;
}