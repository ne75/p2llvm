#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

void a(bool one=false) {
    printf("1\n");

    if (one) return;

    printf("2\n");
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    a();
    a(true);

    printf("~\n"); // end of test character
    return 0;
}