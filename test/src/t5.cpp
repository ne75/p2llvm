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

    struct s_t {
        int a;
        short b;
        char c;
    };

    s_t s = {1, 2, 3};
    printf("%d %d %d\n", s.a, s.b, s.c);

    s_t t = s;
    printf("%d %d %d\n", t.a, t.b, t.c);

    printf("~\n"); // end of test character
    return 0;
}