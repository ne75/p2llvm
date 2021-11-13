#include <stdio.h>
#include <stdlib.h>

unsigned stack[128];

void hub_cog(void *p) {
    printf("%d\n", (int)p);
    busywait();
}

int main() {
    waitx(CLKFREQ/5);
    printf("$\n"); // start of test character
    waitx(CLKFREQ/10);

    cogstart(hub_cog, 1, (int*)stack, sizeof(stack)/4);

    waitx(CLKFREQ/10);
    printf("~\n"); // end of test character

    busywait();

    return 0;
}