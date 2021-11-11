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

    for (int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    int i = 0;
    while (i < 5) {
        printf("%d\n", i++);
    }

    i = 0;
    do {
        printf("%d\n", i++);
    } while (i < 5);

    printf("~\n"); // end of test character
    return 0;
}