#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdlib.h>

#define RX_PIN 63
#define TX_PIN 62

unsigned stack[128];
atomic_t lock;

void hub_cog(void *p) {
    _lock(lock);
    printf("cog 1 has the lock\n");
    waitx(CLKFREQ/10);
    _unlock(lock);
    busywait();
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    waitx(CLKFREQ/5);
    printf("$\n"); // start of test character
    waitx(CLKFREQ/10);

    lock = _locknew();

    cogstart(hub_cog, 1, (int*)stack, sizeof(stack)/4);
    waitx(CLKFREQ/100);
    if (!_locktry(lock)) {
        printf("cog 0 failed to get the lock\n");
    }

    _lock(lock);

    printf("cog 0 has the lock\n");
    printf("~\n"); // end of test character

    busywait();
}