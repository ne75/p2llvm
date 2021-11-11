#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    int a[3];

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;

    printf("%d %d %d", a[0], a[1], a[2]);

    printf("\n~\n"); // end of test character
    return 0;
}