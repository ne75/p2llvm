#include <propeller.h>
#include <stdio.h>
#include <stdint.h>

int test2(int);

int i, j;

int main() {
    i = 0x0a2829;

    test2(i);

    while (1) {
        waitx(CLKFREQ);
    }
}

int test2(int x) {
    char p1, p2;
    p1 = x;
    p2 = (x >> 8);

    asm("drvh %0"::"r"((int)p1));
    asm("drvh %0"::"r"((int)p2));
    asm("drvl %0"::"r"((int)p2));
    asm("drvl %0"::"r"((int)p1));

    return 0;
}