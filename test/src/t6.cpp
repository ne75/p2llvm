#define P2_TARGET_MHZ   300
#include "propeller.h"
#include "sys/p2es_clock.h"

#include <stdio.h>

#define RX_PIN 63
#define TX_PIN 62

class A {
public:
    int a;
    int b;
    static int c;

    A(int _a, int _b) : a(_a), b(_b) {};
    
    ~A() {};
};

int A::c = 3;

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 3000000);
    printf("$\n"); // start of test character

    A s(1, 2);
    A t(4, 5);
    printf("%d %d %d\n", s.a, s.b, s.c);
    printf("%d %d %d\n", t.a, t.b, t.c);

    printf("~\n"); // end of test character
    return 0;
}