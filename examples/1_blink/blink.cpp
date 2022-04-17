#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    drvl(59);

    while(1) {
        drvl(56);
        waitx(CLKFREQ);
    }
}