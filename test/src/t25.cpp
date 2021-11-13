#include <stdio.h>
#include <stdlib.h>
#include <debug.h>

unsigned stack[128];

void hub_cog(void *p) {
    // brk(DEBUG_CODE_INIT);
    printf("%d\n", (int)p);
    busywait();
}

int main() {
    // brk(DEBUG_CODE_INIT);
    
    waitx(CLKFREQ/5);
    printf("$\n"); // start of test character
    waitx(CLKFREQ/10);

    cogstart(hub_cog, 1, (int*)stack, sizeof(stack));

    waitx(CLKFREQ/5);
    printf("~\n"); // end of test character

    busywait();

    return 0;
}