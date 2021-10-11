#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

int a() {
    printf("a()\n");
    return 1;
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);
    printf("$\n"); // start of test character

    int b = a();
    printf("%d\n", b);

    printf("~\n"); // end of test character
    return 0;
}