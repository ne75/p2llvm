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

    dirh(0);
    dirh(1);

    while(1) {
        outnot(0);
        outnot(1);
        waitcnt(CLKFREQ + CNT);
    }
}