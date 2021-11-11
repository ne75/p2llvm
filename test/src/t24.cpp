#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdlib.h>

#define RX_PIN 63
#define TX_PIN 62

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    int start = CNT;
    waitx(CLKFREQ);
    int end = CNT;
    int t = end - start;

    if (abs(t - CLKFREQ) < 100) 
        printf("off by < 100\n");
    else
        printf("off by %d\n", abs(t-CLKFREQ));

    printf("~\n"); // end of test character
    return 0;
}