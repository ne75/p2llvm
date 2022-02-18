#include <stdio.h>
#include <stdlib.h>
#include <debug.h>

unsigned stack[128];

void hub_cog(void *p) {
    printf("%d\n", (int)p);
    busywait();
}

int main() {
    printf("$\n"); // start of test character
    cogstart(hub_cog, 1, (int*)stack, sizeof(stack));

    waitx(CLKFREQ/100);
    printf("~\n"); // end of test character

    busywait();

    return 0;
}