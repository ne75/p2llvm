#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

class A {
public:
    int a;
    int b[2];
    static int c;

    A(int _a, int _b, int _c) {
        a = _a;
        b[0] = _b;
        b[1] = _c;
    };
    
    ~A() {};
};

int A::c = 3;

A s(1, 2, 4);

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    printf("%d %d %d %d\n", s.a, s.b[0], s.b[1], s.c);

    printf("~\n"); // end of test character
    return 0;
}