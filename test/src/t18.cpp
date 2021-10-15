#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

struct s_t {
    char a;
    int b;
};

s_t a(int a, int b, int c, int *d, s_t &e) {
    printf("a(%d, %d, %d, %d, {%d, %d})\n", a, b, c, *d, e.a, e.b);
    s_t r;
    r.a = a;
    r.b = b;
    return r;
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400);
    printf("$\n"); // start of test character

    int b, c, d, e;
    s_t f;
    b = 1;
    c = 2;
    d = 3;
    e = 4;
    f.a = 5;
    f.b = 6;

    s_t r = a(b, c, d, &e, f);
    printf("%d %d\n", r.a, r.b);

    printf("~\n"); // end of test character
    return 0;
}