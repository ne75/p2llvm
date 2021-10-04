/**
 * more function passing tests
 */
#define P2_TARGET_MHZ   200
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

#define RX_PIN 63
#define TX_PIN 62

typedef struct test_t {
    uint32_t a;
} test_t;

void test(int a, int b, int c, int d, int e, test_t &f) {
    printf("%d %d %d %d %d %d\n", a, b, c, d, e, f.a);
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);

    test_t t;
    t.a = 10;

    test(1, 2, 3, 4, 5, t);

    while(1);
}