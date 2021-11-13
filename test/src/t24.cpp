#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("$\n"); // start of test character

    int start = CNT;
    waitx(CLKFREQ);
    int end = CNT;
    int t = end - start;

    if (abs(t - CLKFREQ) < 100) 
        printf("off by < 100\n");
    else
        printf("off by %d\n", abs(t-CLKFREQ));

    printf("~\n"); // end of test character
    return 0;
}