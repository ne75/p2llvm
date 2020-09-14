/*
 * A basic test of using libcalls (for math functions). Eventually expand this example to include floats
 * there's no float hardware so it will take a decent amount of work to implement.
 */
#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

#include "Blinker.h"

#define RX_PIN 63
#define TX_PIN 62

Blinker led1(56, _CLOCKFREQ);
Blinker led2(57, _CLOCKFREQ/2);
Blinker led3(58, _CLOCKFREQ/3);
Blinker led4(59, _CLOCKFREQ/4);

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400); // FIXME: this is SUPPOSED to called by the C standard lib but it isn't working

    led1.start();
    led2.start();
    led3.start();
    led4.start();

    while(1);

}