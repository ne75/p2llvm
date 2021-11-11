#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdarg.h>

#define RX_PIN 63
#define TX_PIN 62

int sum(int n, ...) {
    va_list args;
    va_start(args, n);

    int v = 0;

    for (int i = 0; i < n; i++) {
        v += va_arg(args, int);
    }

    return v;
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    int i = sum(8, 1, -2, 3, -4, 5, -6, 7, -8);
    printf("%d\n", i); // expected -4

    printf("~\n"); // end of test character
    return 0;
}