/*
  test running functions directly in the cog. current limitations:
    - can't use libcalls
    - other stuff?
 */

#define P2_TARGET_MHZ   200
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdarg.h>

#define RX_PIN 63
#define TX_PIN 62

void led_blink(void *p) {

    dirh(56);
    while(1) {
        outnot(56);
        waitx(_CLOCKFREQ);
    }
}

volatile unsigned stack[16];

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);

    //led_blink(); // place the function call just to keep it

    _coginit(0x10, led_blink, (void*)stack);

    while(1) {
        printf("Hello World!\n");
        waitcnt(CLKFREQ + CNT);
    }

    return 0;
}