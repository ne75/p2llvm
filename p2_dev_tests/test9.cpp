/*
  This tests recursion

  compilation:
    make
*/

#define P2_TARGET_MHZ   200
#include "propeller2.h"
#include "sys/p2es_clock.h"

void recurse(int i) {
    if (i == 0) return;

    outl(56);
    waitx(100000000);
    outh(56);
    waitx(100000000);

    recurse(i-1);
}

int main() {
    clkset(_SETFREQ, _CLOCKFREQ);

    dirh(56);
    dirh(57);
    outh(57);

    while(1) {
        recurse(10);
        waitx(400000000);
    }
    return 0;
}