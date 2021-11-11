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

    DIRA = 10;
    printf("%d\n", DIRA); // expect 10
    
    DIRA = 0;
    OUTA = 0;
    dirh(3);
    printf("%d\n", DIRA); // expect 8

    outh(3);
    printf("%d\n", OUTA); // expect 8

    printf("%d\n", INA & 0x8); // expect 8

    printf("~\n"); // end of test character
    return 0;
}