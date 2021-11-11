#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    int a = 0;
    int b = 1;

    if (a) {
        printf("do not print\n");
    } else {
        printf("not a = 0\n");
    }

    if (b) {
        printf("b = 1\n");
    } else {
        printf("do not print\n");
    }

    if (a == b) {
        printf("do not print\n");
    } else {
        printf("not a == b\n");
    }

    if (a != b) {
        printf("a != b\n");
    } else {
        printf("do not print\n");
    }

    if (a > b) {
        printf("do not print\n");
    } else {
        printf("not a > b\n");
    }

    if (a < b) {
        printf("a < b\n");
    } else {
        printf("do not print\n");
    }

    if (a >= b) {
        printf("do not print\n");
    } else {
        printf("not a >= b\n");
    }

    if (a <= b) {
        printf("a <= b\n");
    } else {
        printf("do not print\n");
    }

    printf("~\n"); // end of test character
    return 0;
}