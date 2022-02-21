#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

int main() {
    printf("Hello World!\n");

    while(1) {
        waitx(CLKFREQ);
    }
}