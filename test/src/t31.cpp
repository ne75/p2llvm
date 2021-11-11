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

s_t a(int a, int b, int c, int d, int e) {
    s_t r = {1, 2};
    return r;
}

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    s_t r = a(1, 2, 3, 4, 5);
    printf("%d %d\n", r.a, r.b);

    printf("~\n"); // end of test character
    return 0;
}