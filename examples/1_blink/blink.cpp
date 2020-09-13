/*
 * A propgcc-compatible (almost) blinker program. The main incompatibility is setting up
 * the clock, which will have to be P2 specific, but that's okay since most libraries won't
 * use it.
 *
 */
#include <propeller.h>
#include <sys/p2es_clock.h>

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    DIRB |= 1 << 24;

    while(1) {
        OUTB ^= 1 << 24;
        waitcnt(CLKFREQ + CNT);
    }
}