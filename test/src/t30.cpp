#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

struct a {
    int x[1000];
    int y;
};

a t;

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    t.x[0] = 1;
    t.y = 2;

    printf("%d\n", t.x[0]);
    printf("%d\n", t.y);

    printf("~\n"); // end of test character
    return 0;
}