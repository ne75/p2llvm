#include <stdio.h>
#include <stdlib.h>
#include <propeller.h>
#include <debug.h>

unsigned stack[256];

void hub_cog(void *p) {
    brk(DEBUG_CODE_INIT);

    _lock((int)p);
    printf("cog 1 has the lock\n");
    waitx(CLKFREQ/10);
    _unlock((int)p);
    busywait();
}

int main() {
    printf("$\n"); // start of test character

    int lock = _locknew();
    cogstart(hub_cog, lock, (int*)stack, sizeof(stack));

    waitx(CLKFREQ/100);
    if (!_locktry(lock)) {
        printf("cog 0 failed to get the lock\n");
    }

    _lock(lock);

    printf("cog 0 has the lock\n");
    printf("~\n"); // end of test character

    busywait();
}