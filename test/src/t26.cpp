#include <propeller.h>
#include <debug.h>

#include <stdio.h>
#include <stdlib.h>

unsigned stack[256];

__attribute__ ((cogmain, noreturn)) void cog(void *p) {
    brk(0);
    INIT_RTLIB;
    int val = (int)p;
    printf("%d\n", val/-3);
    busywait();
}

int main() {
    hubset(DEBUG_INT_EN | DEBUG_COG1 | DEBUG_COG0);
    brk(0);
    printf("$\n"); // start of test character

    int v = 1;
    stack[0] = 15;
    _coginit(0x10, cog, stack);

    waitx(CLKFREQ/10);
    printf("~\n"); // end of test character

    busywait();

    return 0;
}