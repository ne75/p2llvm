#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    drvl(59);

    while(1) {
        drvl(56);
        waitx(CLKFREQ/10000);
        drvh(56);
        waitx(24*(CLKFREQ/10000));
    }
    // int f = 25;
    // int t = _CLOCKFREQ/(2*f*6);
    // while(1) {
    //     for (int i = 56; i < 62; i++) {
    //         drvl(i);
    //         waitx(t);
    //         drvh(i);
    //     }
    // }
}