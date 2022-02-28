/*
 * A basic test of using libcalls (for math functions). Eventually expand this example to include floats
 * there's no float hardware so it will take a decent amount of work to implement.
 */
#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

#include "Blinker.h"

Blinker led1(56, CLKFREQ);
Blinker led2(57, CLKFREQ/2);
Blinker led3(58, CLKFREQ/3);
Blinker led4(59, CLKFREQ/4);

int main() {
    led1.start();
    led2.start();
    led3.start();
    led4.start();

    while(1) {
        waitx(CLKFREQ);
    }

}